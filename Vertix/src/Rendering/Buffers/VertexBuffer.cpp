//
// Created by Natsurainko on 2026/2/2.
//

#include "Vertix/Rendering/Buffers/VertexBuffer.h"

Vertix::VertexBuffer::VertexBuffer(
    const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
    const uint32_t                                vertexCount,
    const uint32_t                                strideSize)
: RenderResource(RenderResourceKind::Buffer, d3d12Resource, D3D12_RESOURCE_STATE_COMMON),
  vertexCount(vertexCount),
  strideSize(strideSize) {
    bufferView = D3D12_VERTEX_BUFFER_VIEW {
        .BufferLocation = d3d12Resource->GetGPUVirtualAddress(),
        .SizeInBytes = strideSize * vertexCount,
        .StrideInBytes = strideSize
    };
}

std::unique_ptr<Vertix::VertexBuffer> Vertix::VertexBuffer::Create(
    const void*                data,
    const uint32_t             strideSize,
    const uint32_t             vertexCount,
    ID3D12Device*              device,
    ID3D12GraphicsCommandList* commandList,
    ResourceUploadHeap &       resourceUploadHeap) {
    Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;

    const uint64_t bufferSize   = vertexCount * strideSize;
    const auto     resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    const auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(
        device->CreateCommittedResource(
            &defaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&(bufferResource))
        )
    );

    resourceUploadHeap.CommitUploadResource(
        device,
        resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        [&](ID3D12Resource* uploadResource) {
            const D3D12_SUBRESOURCE_DATA subresourceData = {
                .pData = data,
                .RowPitch = static_cast<LONG_PTR>(bufferSize),
                .SlicePitch = static_cast<LONG_PTR>(bufferSize),
            };

            UpdateSubresources(
                commandList,
                bufferResource.Get(),
                uploadResource,
                0,
                0,
                1,
                &subresourceData
            );
        }
    );
    
    return std::make_unique<VertexBuffer>(std::move(bufferResource), vertexCount, strideSize);
}
