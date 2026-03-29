//
// Created by Natsurainko on 2026/3/28.
//

#include "Graphics/Raytracing/TopLevelAccelerationStructure.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>

#include "Exceptions/HResultException.h"

Vertix::TopLevelAccelerationStructure* Vertix::TopLevelAccelerationStructure::Create(
    const Microsoft::WRL::ComPtr<ID3D12Device5> &d3d12Device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> &commandList,
    const std::vector<D3D12_RAYTRACING_INSTANCE_DESC> &instanceDescs)
{
    auto tlas = new TopLevelAccelerationStructure();

    const auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    {
        const UINT64 instanceBufferSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceDescs.size();
        const auto instanceBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(instanceBufferSize);

        ThrowIfFailed(d3d12Device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &instanceBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&tlas->InstancesBufferResource)));

        void* pData;
        ThrowIfFailed(tlas->InstancesBufferResource->Map(0, nullptr, &pData));
        memcpy(pData, instanceDescs.data(), instanceBufferSize);
        tlas->InstancesBufferResource->Unmap(0, nullptr);
    }

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs = static_cast<UINT>(instanceDescs.size());
    inputs.InstanceDescs = tlas->InstancesBufferResource->GetGPUVirtualAddress();

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
    d3d12Device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    const auto tlasBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    const auto tlasScratchBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(info.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &tlasBufferDesc,
        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
        nullptr,
        IID_PPV_ARGS(&tlas->TLASResource)));

    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &tlasScratchBufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&tlas->TLASScratchResource)));

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
    buildDesc.Inputs = inputs;
    buildDesc.DestAccelerationStructureData = tlas->TLASResource->GetGPUVirtualAddress();
    buildDesc.ScratchAccelerationStructureData = tlas->TLASScratchResource->GetGPUVirtualAddress();
    buildDesc.SourceAccelerationStructureData = 0;

    commandList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
    const auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(nullptr);
    commandList->ResourceBarrier(1, &uavBarrier);

    return tlas;
}
