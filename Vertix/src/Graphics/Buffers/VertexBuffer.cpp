//
// Created by Natsurainko on 2026/2/2.
//

#include "Graphics/Buffers/VertexBuffer.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_resource_helpers.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsCommandList.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ResourceUploadHeap.h"

Vertix::VertexBuffer* Vertix::VertexBuffer::Create(const std::vector<Vertex> &vertices,
                                                   const GraphicsDevice* graphicsDevice,
                                                   const GraphicsCommandList* graphicsCommandList,
                                                   ResourceUploadHeap &resourceUploadHeap) {
    const auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    const auto &device = graphicsDevice->GetD3D12Device();
    const auto &commandList = graphicsCommandList->GetD3D12GraphicsCommandList();
    const auto buffer = new VertexBuffer();

    Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;

    {
        const UINT64 vertexBufferSize = vertices.size() * sizeof(Vertex);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

        ThrowIfFailed(device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&(buffer->d3d12Resource))));

        const CD3DX12_RESOURCE_BARRIER commonToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
            buffer->d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST);
        commandList->ResourceBarrier(1, &commonToCopyDest);

        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadResource)));

        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = vertices.data();
        subresourceData.RowPitch = static_cast<LONG_PTR>(vertexBufferSize);
        subresourceData.SlicePitch = static_cast<LONG_PTR>(vertexBufferSize);

        UpdateSubresources(commandList.Get(),
            buffer->d3d12Resource.Get(),
            uploadResource.Get(),
            0, 0, 1, &subresourceData);

        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            buffer->d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        commandList->ResourceBarrier(1, &barrier);

        buffer->d3d12VertexBufferView.BufferLocation = buffer->d3d12Resource->GetGPUVirtualAddress();
        buffer->d3d12VertexBufferView.StrideInBytes = sizeof(Vertex);
        buffer->d3d12VertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
        buffer->vertexCount = vertices.size();

        resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(uploadResource));
    }

    return buffer;
}
