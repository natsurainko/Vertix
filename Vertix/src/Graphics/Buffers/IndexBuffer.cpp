//
// Created by Natsurainko on 2026/3/22.
//

#include "Graphics/Buffers/IndexBuffer.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_resource_helpers.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsCommandList.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ResourceUploadHeap.hpp"

Vertix::IndexBuffer* Vertix::IndexBuffer::Create(
    const std::vector<UINT32> &indices,
    const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
    ResourceUploadHeap &resourceUploadHeap)
{
    const auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto buffer = new IndexBuffer();

    Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;

    {
        const UINT64 indexBufferSize = indices.size() * sizeof(UINT32);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

        ThrowIfFailed(d3d12Device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&(buffer->d3d12Resource))));

        ThrowIfFailed(d3d12Device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadResource)));

        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = indices.data();
        subresourceData.RowPitch = static_cast<LONG_PTR>(indexBufferSize);
        subresourceData.SlicePitch = static_cast<LONG_PTR>(indexBufferSize);

        UpdateSubresources(commandList.Get(),
            buffer->d3d12Resource.Get(),
            uploadResource.Get(),
            0, 0, 1, &subresourceData);

        buffer->d3d12IndexBufferView.BufferLocation = buffer->d3d12Resource->GetGPUVirtualAddress();
        buffer->d3d12IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
        buffer->d3d12IndexBufferView.SizeInBytes = static_cast<UINT>(indexBufferSize);
        buffer->indexCount = indices.size();

        resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(uploadResource));
    }

    return buffer;
}