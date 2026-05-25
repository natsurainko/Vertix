//
// Created by Natsurainko on 2026/5/16.
//

#include "Vertix/Rendering/Pipeline/RenderPipeline.h"

void Vertix::RenderPipeline::Execute() {
    auto* currentFrameResource = swapChain->GetCurrentBuffer()->GetResource();
    const auto barrierToRT = CD3DX12_RESOURCE_BARRIER::Transition(
        currentFrameResource,
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    const auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
        currentFrameResource,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    currentFrameRTV = &frameRTVs[swapChain->GetCurrentFrameIndex()];
    for (const auto& frameInjector : frameInjectors) {
        frameInjector->InjectValue(currentFrameRTV);
    }

    descriptorHeapSet->SetDescriptorHeaps(d3d12CommandList);
    d3d12CommandList->RSSetViewports(1, &viewport);
    d3d12CommandList->RSSetScissorRects(1, &scissorRect);
    d3d12CommandList->ResourceBarrier(1, &barrierToRT);
    {
        for (UINT i = 0; i < passes.size(); ++i) {
            auto &preBarriers = prePassBarriers[i];
            if (!preBarriers.empty()) {
                d3d12CommandList->ResourceBarrier(static_cast<UINT>(preBarriers.size()), preBarriers.data());
            }

            passes[i]->Execute(d3d12CommandList);
        }
        d3d12CommandList->ResourceBarrier(static_cast<UINT>(postPassBarriers.size()), postPassBarriers.data());
    }
    d3d12CommandList->ResourceBarrier(1, &barrierToPresent);
}

void Vertix::RenderPipeline::Resize(const Vector2D<UINT> &size) {
    frameCommandList->WaitForCommand();

    viewport.Width = static_cast<float>(size.X);
    viewport.Height = static_cast<float>(size.Y);
    scissorRect.right = static_cast<LONG>(size.X);
    scissorRect.bottom = static_cast<LONG>(size.Y);

    swapChain->Resize(size);

    for (UINT i = 0; i < swapChain->GetFrameCount(); ++i) {
        d3d12Device->CreateRenderTargetView(
            swapChain->GetBuffer(i)->GetResource(),
            swapChainViewDesc.has_value() ? &swapChainViewDesc.value() : nullptr,
            frameRTVs[i].cpuHandle
        );
    }

    const uint64_t size_[3] = { size.X, size.Y, 0 };
    for (const auto &[resourceName, texture] : resizableTextures) {
        texture->Resize(d3d12Device, size_);
        for (auto &[handle, factory] : descriptorViews.at(resourceName)) {
            factory(d3d12Device, handle, resources);
        }
    }

    CompileBarriers();
}

void Vertix::RenderPipeline::CompileBarriers() {
    passes.clear();
    prePassBarriers.assign(pipelineGraphNodes.size(), {});
    postPassBarriers.clear();

    auto currentStates = resourcesInitialStates;

    for (UINT i = 0; i < pipelineGraphNodes.size() - 1; ++i) {
        const auto* node = pipelineGraphNodes[i];
        passes.push_back(node->renderPass.get());

        auto addTransitionBarrier = [&](const std::string& resName, const D3D12_RESOURCE_STATES before, const D3D12_RESOURCE_STATES after){
            auto* d3d12Res = resources.at(resName)->GetResource();
            prePassBarriers[i].push_back(CD3DX12_RESOURCE_BARRIER::Transition(d3d12Res, before, after));
        };
        auto addUAVBarrier = [&](const std::string& resName) {
            auto* d3d12Res = resources.at(resName)->GetResource();
            prePassBarriers[i].push_back(CD3DX12_RESOURCE_BARRIER::UAV(d3d12Res));
        };

        for (const auto* edge : node->inEdges) {
            if (edge->resourceName.empty() || edge->resourceName == swapChainResourceName) continue;
            auto& cur = currentStates[edge->resourceName];
            if (cur != edge->readerState) {
                addTransitionBarrier(edge->resourceName, cur, edge->readerState);
                cur = edge->readerState;
            } else if (cur & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
                addUAVBarrier(edge->resourceName);
            }
        }

        for (const auto* edge : node->outEdges) {
            if (edge->resourceName.empty() || edge->resourceName == swapChainResourceName) continue;
            auto& cur = currentStates[edge->resourceName];
            if (cur != edge->writerState) {
                addTransitionBarrier(edge->resourceName, cur, edge->writerState);
                cur = edge->writerState;
            } else if (cur & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
                addUAVBarrier(edge->resourceName);
            }
        }
    }

    for (auto& [resName, finalState] : currentStates) {
        if (const D3D12_RESOURCE_STATES initState = resourcesInitialStates.at(resName); finalState != initState) {
            auto* d3d12Res = resources.at(resName)->GetResource();
            postPassBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(d3d12Res, finalState, initState));
        }
    }
}
