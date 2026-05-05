//
// Created by Natsurainko on 2026/3/29.
//

#include "LightingPass.h"

#include <LightingPass_PS.h>
#include <LightingPass_VS.h>
#include <d3d12/d3dx12_root_signature.h>

#include "Vertix/Graphics/SwapChain.h"

LightingPass::LightingPass(Vertix::SwapChain *swapChain) : swapChain(swapChain) {}

void LightingPass::Initialize(
    Vertix::GraphicsDevice* device,
    RenderContext *context)
{
    RenderPass::Initialize(device, context);
    const auto &d3d12Device = device->GetD3D12Device();

    shadowMaskSRV = renderContext->renderTextureAllocator->CreateShaderResourceView(renderContext->shadowMaskTexture);

    {
        CD3DX12_DESCRIPTOR_RANGE srvRanges[1];
        srvRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_STATIC_SAMPLER_DESC staticSampler(0);

        CD3DX12_ROOT_PARAMETER rootParameters[1];
        rootParameters[0].InitAsDescriptorTable(1, &srvRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.NumParameters = 1;
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.pStaticSamplers = &staticSampler;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

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
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertix::Vertex, TexCoord), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
        psoDesc.pRootSignature = rootSignature.Get();
        psoDesc.VS = SHADER_BYTECODE(SHADER_BYTECODE_LIGHTING_PASS_VS);
        psoDesc.PS = SHADER_BYTECODE(SHADER_BYTECODE_LIGHTING_PASS_PS);
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC2(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        psoDesc.NumRenderTargets = 1;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleMask = UINT_MAX;
        ThrowIfFailed(d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
    }
}

void LightingPass::Execute(ID3D12GraphicsCommandList5* commandList) {
    constexpr float clearColor[] = { 0.127437680f, 0.300543794f, 0.846873232f, 1.0f };
    const auto scopedTransition = renderContext->shadowMaskTexture->ScopedTransition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    {
        commandList->SetDescriptorHeaps(1, renderContext->renderTextureAllocator->GetShaderResourceDescriptorHeap()->GetDescriptorHeap().GetAddressOf());
        commandList->SetGraphicsRootSignature(rootSignature.Get());
        commandList->SetGraphicsRootDescriptorTable(0, shadowMaskSRV.GetGpuHandle());

        renderContext->currentRenderTargetView->SetRenderTarget(commandList);
        renderContext->currentRenderTargetView->Clear(commandList, clearColor);

        commandList->SetPipelineState(pipelineState.Get());
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        commandList->IASetVertexBuffers(0, 1, &renderContext->fullScreenVertex->d3d12VertexBufferView);
        commandList->DrawInstanced(renderContext->fullScreenVertex->vertexCount, 1, 0, 0);
    }
}

void LightingPass::Resize(const Vertix::Vector2D<unsigned> &size) {
    const auto &d3d12Device = graphicsDevice->GetD3D12Device();
    shadowMaskSRV.Reuse(d3d12Device, renderContext->shadowMaskTexture->GetResource());
}
