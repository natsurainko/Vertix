//
// Created by Natsurainko on 2026/1/27.
//

#include "GeometryPass.h"

#include <GeometryPass_PS.h>
#include <GeometryPass_VS.h>
#include <d3d12/d3dx12_core.h>
#include <d3d12/d3dx12_root_signature.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Graphics/GraphicsDevice.h"

GeometryPass::~GeometryPass() {
    delete depthStencilView;

    for (const auto &renderTargetView : renderTargetViews) {
        delete renderTargetView;
    }
}

void GeometryPass::Initialize(
    Vertix::GraphicsDevice* device,
    RenderContext *context)
{
    RenderPass::Initialize(device, context);
    const auto &d3d12Device = device->GetD3D12Device();

    {
        renderContext->rtvDescriptorHeap.AllocDescriptorHandle(rtvHandles, renderTargetFormats.size());
        renderContext->dsvDescriptorHeap.AllocDescriptorHandle(dsvHandle);
        renderContext->srvUavDescriptorHeap.AllocDescriptorHandle(srvHandles, renderContext->geometrySrvGpuHandles, renderTargetFormats.size());
    }

    {
        auto dsvResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT,context->windowSize.X, context->windowSize.Y);
        dsvResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        depthStencilView = new Vertix::DepthStencilView(graphicsDevice, dsvResDesc, dsvHandle);

        for (int i = 0; i < renderTargetFormats.size(); i++) {
            const auto& format = renderTargetFormats[i];
            auto rtvResDesc = CD3DX12_RESOURCE_DESC::Tex2D(format,context->windowSize.X, context->windowSize.Y);
            auto srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2D(format, 1);
            rtvResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

            renderTargetViews[i] = new Vertix::RenderTargetView(graphicsDevice, rtvResDesc, rtvHandles[i]);
            renderTargetViews[i]->CreateShaderResourceView(&srvDesc, srvHandles[i]);
            renderContext->geometryRtvBarriers[i] = renderTargetViews[i]->CreateTransitionBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
            srvBarriers[i] = renderTargetViews[i]->CreateTransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        }
    }

    {
        CD3DX12_STATIC_SAMPLER_DESC staticSampler(0);
        staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        staticSampler.Filter = D3D12_FILTER_ANISOTROPIC;
        staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.MaxAnisotropy = 4;
        staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        CD3DX12_ROOT_PARAMETER rootParameters[4];
        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[2].InitAsConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[3].InitAsShaderResourceView(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.NumParameters = 4;
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.pStaticSamplers = &staticSampler;
        rootSignatureDesc.Flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

        ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(d3d12Device->CreateRootSignature(0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature)));
    }

    {
        constexpr D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertix::Vertex, Position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertix::Vertex, Normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertix::Vertex, TexCoord), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertix::Vertex, Tangent), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertix::Vertex, Bitangent), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
        psoDesc.pRootSignature = rootSignature.Get();
        psoDesc.VS = SHADER_BYTECODE(SHADER_BYTECODE_GEOMETRY_PASS_VS);
        psoDesc.PS = SHADER_BYTECODE(SHADER_BYTECODE_GEOMETRY_PASS_PS);
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC2(D3D12_DEFAULT);
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleMask = UINT_MAX;

        psoDesc.NumRenderTargets = renderTargetFormats.size();
        for (int i = 0; i < renderTargetFormats.size(); i++) {
            psoDesc.RTVFormats[i] = renderTargetFormats[i];
        }

        ThrowIfFailed(d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
    }
}

void GeometryPass::Execute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) {
    constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    // commandList->SetDescriptorHeaps(1, renderContext->texturePool.GetDescriptorHeap().GetAddressOf());
    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->SetGraphicsRootConstantBufferView(0, renderContext->frameConstantsBuffer.GetGpuVirtualAddress());
    // commandList->SetGraphicsRootShaderResourceView(3, renderContext->materialPool.GetGpuVirtualAddress());

    commandList->OMSetRenderTargets(renderTargetFormats.size(), &rtvHandles[0], FALSE, &dsvHandle);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);
    for (const auto rtvHandle : rtvHandles) {
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    }

    commandList->SetPipelineState(pipelineState.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (UINT i = 0; i < renderContext->sceneObjects.size(); ++i) {
        const auto &sceneObject = renderContext->sceneObjects[i];
        commandList->SetGraphicsRootConstantBufferView(1, renderContext->objectConstantsBuffer.GetGpuVirtualAddressAt(i));

        for (const auto &mesh : sceneObject->SceneModel->Meshes) {
            commandList->SetGraphicsRoot32BitConstant(2, mesh.Material.slot, 0);
            commandList->IASetVertexBuffers(0, 1, &mesh.VertexBuffer->d3d12VertexBufferView);
            commandList->IASetIndexBuffer(&mesh.IndexBuffer->d3d12IndexBufferView);
            commandList->DrawIndexedInstanced(mesh.IndexBuffer->indexCount, 1, 0, 0, 0);
        }
    }

    commandList->ResourceBarrier(renderTargetFormats.size(), srvBarriers);
}

void GeometryPass::Resize(const Vertix::Vector2D<unsigned> &size) {
    depthStencilView->Resize(size);

    for (int i = 0; i < renderTargetFormats.size(); i++) {
        const auto& format = renderTargetFormats[i];
        auto srvDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::Tex2D(format, 1);

        renderTargetViews[i]->Resize(size);
        renderTargetViews[i]->CreateShaderResourceView(&srvDesc, srvHandles[i]);
        renderContext->geometryRtvBarriers[i] = renderTargetViews[i]->CreateTransitionBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
        srvBarriers[i] = renderTargetViews[i]->CreateTransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}
