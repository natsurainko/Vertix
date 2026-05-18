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

void GeometryPass::Initialize(ID3D12Device10* device) {
    {
        CD3DX12_ROOT_PARAMETER rootParameters[2];
        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.NumParameters = 2;
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumStaticSamplers = 0;
        rootSignatureDesc.pStaticSamplers = nullptr;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;

        ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(device->CreateRootSignature(0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature)));
    }

    {
        constexpr D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertix::Vertex, Position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertix::Vertex, Normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.NumRenderTargets = 2;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        psoDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;

        ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
    }
}

void GeometryPass::Execute(ID3D12GraphicsCommandList5* commandList) {
    constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[2] = { gPositionDepthRTV->cpuHandle, gNormalRoughnessRTV->cpuHandle };

    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->SetGraphicsRootConstantBufferView(0, renderContext->frameConstantsBuffer->GetGPUVirtualAddress());
    commandList->OMSetRenderTargets(2, renderTargets, FALSE, &gDepthDSV->cpuHandle);

    gPositionDepthRTV->Clear(commandList, clearColor);
    gNormalRoughnessRTV->Clear(commandList, clearColor);
    gDepthDSV->ClearDepth(commandList);

    commandList->SetPipelineState(pipelineState.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (UINT i = 0; i < renderContext->sceneObjects.size(); ++i) {
        const auto &sceneObject = renderContext->sceneObjects[i];
        commandList->SetGraphicsRootConstantBufferView(1, renderContext->objectConstantsBuffer->GetGpuVirtualAddressAt(i));

        for (const auto &mesh : sceneObject->SceneModel->Meshes) {
            commandList->IASetVertexBuffers(0, 1, &mesh.VertexBuffer->d3d12VertexBufferView);
            commandList->IASetIndexBuffer(&mesh.IndexBuffer->d3d12IndexBufferView);
            commandList->DrawIndexedInstanced(mesh.IndexBuffer->indexCount, 1, 0, 0, 0);
        }
    }
}
