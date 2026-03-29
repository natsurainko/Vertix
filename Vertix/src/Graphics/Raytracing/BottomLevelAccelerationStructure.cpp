//
// Created by Natsurainko on 2026/3/28.
//

#include "Graphics/Raytracing/BottomLevelAccelerationStructure.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>

#include "Exceptions/HResultException.h"
#include "Primitive/Mesh.h"

Vertix::BottomLevelAccelerationStructure* Vertix::BottomLevelAccelerationStructure::Create(
    const Microsoft::WRL::ComPtr<ID3D12Device5> &d3d12Device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> &commandList,
    Mesh* mesh)
{
    assert(mesh);
    assert(mesh->VertexBuffer);
    assert(mesh->IndexBuffer);

    auto blas = new BottomLevelAccelerationStructure();

    D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = {};
    geomDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;

    geomDesc.Triangles.VertexBuffer.StartAddress = mesh->VertexBuffer->d3d12Resource->GetGPUVirtualAddress();
    geomDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);
    geomDesc.Triangles.VertexCount = mesh->VertexBuffer->vertexCount;
    geomDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;

    geomDesc.Triangles.IndexBuffer = mesh->IndexBuffer->d3d12Resource->GetGPUVirtualAddress();
    geomDesc.Triangles.IndexCount = mesh->IndexBuffer->indexCount;
    geomDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

    geomDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs = 1;
    inputs.pGeometryDescs = &geomDesc;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
    d3d12Device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    const auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto blasBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    const auto blasScratchBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(info.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &blasBufferDesc,
        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
        nullptr,
        IID_PPV_ARGS(&blas->BLASResource)));
    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &blasScratchBufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&blas->BLASScratchResource)));

    mesh->VertexBuffer->d3d12Resource->SetName(L"BLAS Vertex Buffer");
    mesh->IndexBuffer->d3d12Resource->SetName(L"BLAS Index Buffer");
    blas->BLASResource->SetName(L"BLAS Resource Buffer");

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
    buildDesc.Inputs = inputs;
    buildDesc.DestAccelerationStructureData = blas->BLASResource->GetGPUVirtualAddress();
    buildDesc.ScratchAccelerationStructureData = blas->BLASScratchResource->GetGPUVirtualAddress();

    commandList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

    const auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(nullptr);
    commandList->ResourceBarrier(1, &uavBarrier);

    return blas;
}
