//
// Created by Natsurainko on 2026/5/30.
//

#include "Vertix/Rendering/Buffers/AccelerationStructure.h"

#include "Vertix/Rendering/Buffers/IndexBuffer.h"
#include "Vertix/Rendering/Buffers/VertexBuffer.h"

#if VERTIX_D3D12_DEVICE_VERSION >= 5 && VERTIX_D3D12_COMMAND_LIST_VERSION >= 5

Vertix::AccelerationStructure::AccelerationStructure(
    const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE type,
    const Microsoft::WRL::ComPtr<ID3D12Resource> &     resource,
    const Microsoft::WRL::ComPtr<ID3D12Resource> &     scratchBuffer)
: RenderBuffer(resource, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE),
  type(type),
  scratchBuffer(scratchBuffer) {}

std::unique_ptr<Vertix::AccelerationStructure> Vertix::AccelerationStructure::CreateBottomLevelAccelerationStructure(
    D3D12Interface::Device*      device,
    D3D12Interface::CommandList* commandList,
    const VertexBuffer*          vertexBuffer,
    const IndexBuffer*           indexBuffer,
    const DXGI_FORMAT            positionFormat) {
    D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = {};

    geomDesc.Triangles.VertexBuffer.StartAddress  = vertexBuffer->GetGPUVirtualAddress();
    geomDesc.Triangles.VertexBuffer.StrideInBytes = vertexBuffer->GetStrideSize();
    geomDesc.Triangles.VertexCount                = vertexBuffer->GetVertexCount();
    geomDesc.Triangles.VertexFormat               = positionFormat;

    geomDesc.Triangles.IndexBuffer = indexBuffer->GetGPUVirtualAddress();
    geomDesc.Triangles.IndexCount  = indexBuffer->GetIndexCount();
    geomDesc.Triangles.IndexFormat = indexBuffer->GetFormat();

    geomDesc.Type  = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geomDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

    const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {
        .Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL,
        .NumDescs = 1,
        .DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY,
        .pGeometryDescs = &geomDesc
    };

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

    const auto defaultHeap       = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto bufferDesc        = CD3DX12_RESOURCE_DESC::Buffer(prebuildInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    const auto scratchBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(prebuildInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer;

    ThrowIfFailed(
        device->CreateCommittedResource(
            &defaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
            nullptr,
            IID_PPV_ARGS(&buffer)
        )
    );

    ThrowIfFailed(
        device->CreateCommittedResource(
            &defaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &scratchBufferDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&scratchBuffer)
        )
    );

    const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {
        .DestAccelerationStructureData = buffer->GetGPUVirtualAddress(),
        .Inputs = inputs,
        .ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress(),
    };

    commandList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
    const auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(buffer.Get());
    commandList->ResourceBarrier(1, &uavBarrier);

    return std::unique_ptr<AccelerationStructure>(
        new AccelerationStructure(
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL,
            std::move(buffer),
            std::move(scratchBuffer)
        )
    );
}

std::unique_ptr<Vertix::AccelerationStructure> Vertix::AccelerationStructure::CreateTopLevelAccelerationStructure(
    D3D12Interface::Device*                            device,
    D3D12Interface::CommandList*                       commandList,
    ResourceUploadHeap &                               resourceUploadHeap,
    const std::vector<D3D12_RAYTRACING_INSTANCE_DESC> &instanceDescs) {
    const uint64_t instanceBufferSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instanceDescs.size();

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {
        .Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL,
        .Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE,
        .NumDescs = static_cast<UINT>(instanceDescs.size()),
        .DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY,
    };

    resourceUploadHeap.CommitUploadResource(
        device,
        CD3DX12_RESOURCE_DESC::Buffer(instanceBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        [&](ID3D12Resource* uploadResource) {
            inputs.InstanceDescs = uploadResource->GetGPUVirtualAddress();

            void* pData;
            ThrowIfFailed(uploadResource->Map(0, nullptr, &pData));
            memcpy(pData, instanceDescs.data(), instanceBufferSize);
            uploadResource->Unmap(0, nullptr);
        }
    );

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &prebuildInfo);

    const auto defaultHeap       = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto bufferDesc        = CD3DX12_RESOURCE_DESC::Buffer(prebuildInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    const auto scratchBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(prebuildInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer;

    ThrowIfFailed(
        device->CreateCommittedResource(
            &defaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
            nullptr,
            IID_PPV_ARGS(&buffer)
        )
    );

    ThrowIfFailed(
        device->CreateCommittedResource(
            &defaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &scratchBufferDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&scratchBuffer)
        )
    );

    const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {
        .DestAccelerationStructureData = buffer->GetGPUVirtualAddress(),
        .Inputs = inputs,
        .ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress(),
    };

    commandList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
    const auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(buffer.Get());
    commandList->ResourceBarrier(1, &uavBarrier);

    return std::unique_ptr<AccelerationStructure>(
        new AccelerationStructure(
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL,
            std::move(buffer),
            std::move(scratchBuffer)
        )
    );
}


#endif
