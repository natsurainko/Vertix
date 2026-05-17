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

    frameRTV = &frameRTVs[swapChain->GetCurrentFrameIndex()];
    for (const auto& frameInjector : frameInjectors) {
        frameInjector->Inject(frameRTV);
    }

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
        frameRTVs[i].RecreateView(d3d12Device, swapChain->GetBuffer(i), swapChainViewDesc);
    }

    const uint64_t size_[3] = { size.X, size.Y, 0 };
    for (const auto &[resourceName, texture] : resizableTextures) {
        texture->Resize(d3d12Device, size_);
        for (auto &viewDesc : resourcesViewDescs.at(resourceName)) {
            RecreateTextureView(texture, viewDesc, views.at(viewDesc));
        }
    }

    CompileBarriers();
}

void Vertix::RenderPipeline::CompileBarriers() {
    passes.clear();
    prePassBarriers.assign(pipelineGraphNodes.size(), {});
    postPassBarriers.clear();

    auto currentStates = resourcesInitialStates;

    for (UINT i = 0; i < static_cast<UINT>(pipelineGraphNodes.size()); ++i) {
        const auto* node = pipelineGraphNodes[i];
        passes.push_back(node->renderPass.get());

        auto addTransitionBarrier = [&](const std::string& resName, const D3D12_RESOURCE_STATES before, const D3D12_RESOURCE_STATES after){
            auto* d3d12Res = resources.at(resName)->GetResource();
            prePassBarriers[i].push_back(CD3DX12_RESOURCE_BARRIER::Transition(d3d12Res, before, after));
        };

        for (const auto& edge : node->inEdges) {
            if (edge->resourceName.empty() || edge->resourceName == swapChainResourceName) continue;
            auto& cur = currentStates[edge->resourceName];
            if (cur != edge->readerState) {
                addTransitionBarrier(edge->resourceName, cur, edge->readerState);
                cur = edge->readerState;
            }
        }

        for (const auto& edge : node->outEdges) {
            if (edge->resourceName.empty() || edge->resourceName == swapChainResourceName) continue;
            auto& cur = currentStates[edge->resourceName];
            if (cur != edge->writerState) {
                addTransitionBarrier(edge->resourceName, cur, edge->writerState);
                cur = edge->writerState;
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

void Vertix::RenderPipeline::RecreateTextureView(
    const RenderTexture *resource,
    const RenderResourceViewDesc *viewDesc,
    const DescriptorHeapHandle &handle) const
{
    if (std::holds_alternative<std::monostate>(viewDesc->desc)) {
        switch (viewDesc->type) {
            case RenderResourceViewType::RenderTarget:    d3d12Device->CreateRenderTargetView(resource->GetResource(), nullptr, handle.cpuHandle); break;
            case RenderResourceViewType::DepthStencil:    d3d12Device->CreateDepthStencilView(resource->GetResource(), nullptr, handle.cpuHandle); break;
            case RenderResourceViewType::UnorderedAccess: d3d12Device->CreateUnorderedAccessView(resource->GetResource(), nullptr, nullptr, handle.cpuHandle); break;
            case RenderResourceViewType::ShaderResource:  d3d12Device->CreateShaderResourceView(resource->GetResource(), nullptr, handle.cpuHandle); break;
        }
        return;
    }

    const void* descPtr = &viewDesc->desc;
    switch (viewDesc->type) {
        case RenderResourceViewType::RenderTarget:
            d3d12Device->CreateRenderTargetView(resource->GetResource(), static_cast<const D3D12_RENDER_TARGET_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
        case RenderResourceViewType::DepthStencil:
            d3d12Device->CreateDepthStencilView(resource->GetResource(), static_cast<const D3D12_DEPTH_STENCIL_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
        case RenderResourceViewType::UnorderedAccess:
            d3d12Device->CreateUnorderedAccessView(resource->GetResource(), nullptr, static_cast<const D3D12_UNORDERED_ACCESS_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
        case RenderResourceViewType::ShaderResource:
            d3d12Device->CreateShaderResourceView(resource->GetResource(), static_cast<const D3D12_SHADER_RESOURCE_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
    }
}
