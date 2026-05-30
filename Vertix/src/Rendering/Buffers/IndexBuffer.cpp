//
// Created by Natsurainko on 2026/3/22.
//

#include "Vertix/Rendering/Buffers/IndexBuffer.h"

Vertix::IndexBuffer::IndexBuffer(
    const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
    const uint32_t                                indexCount,
    const DXGI_FORMAT                             format)
: RenderResource(RenderResourceKind::Buffer, d3d12Resource, D3D12_RESOURCE_STATE_COMMON),
  format(format),
  indexCount(indexCount) {
    bufferView = D3D12_INDEX_BUFFER_VIEW {
        .BufferLocation = d3d12Resource->GetGPUVirtualAddress(),
        .SizeInBytes = static_cast<uint32_t>(sizeof(uint32_t) * indexCount),
        .Format = format,
    };
}

std::unique_ptr<Vertix::IndexBuffer> Vertix::IndexBuffer::Create(
    const void*                data,
    const uint32_t             indexCount,
    const DXGI_FORMAT          format,
    ID3D12Device*              device,
    ID3D12GraphicsCommandList* commandList,
    ResourceUploadHeap &       resourceUploadHeap) {
    Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;

    const uint64_t bufferSize   = indexCount * GetStrideSize(format);
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

    return std::make_unique<IndexBuffer>(std::move(bufferResource), indexCount, format);
}

uint32_t Vertix::IndexBuffer::GetStrideSize(const DXGI_FORMAT format) {
    switch (format) {
        case DXGI_FORMAT_R16_UINT: return sizeof(uint16_t);
        case DXGI_FORMAT_R32_UINT: return sizeof(uint32_t);
        default: throw std::runtime_error("This DXGI_FORMAT can't be used for IndexBuffer.");
    }
}
