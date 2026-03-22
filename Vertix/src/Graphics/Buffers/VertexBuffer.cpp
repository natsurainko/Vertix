//
// Created by Natsurainko on 2026/2/2.
//

#include "Graphics/Buffers/VertexBuffer.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_resource_helpers.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsCommandList.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ResourceUploadHeap.hpp"

Vertix::VertexBuffer* Vertix::VertexBuffer::Create(
    const std::vector<Vertex> &vertices,
    const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
    ResourceUploadHeap &resourceUploadHeap)
{
    const auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto buffer = new VertexBuffer();

    Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;

    {
        const UINT64 vertexBufferSize = vertices.size() * sizeof(Vertex);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

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
        subresourceData.pData = vertices.data();
        subresourceData.RowPitch = static_cast<LONG_PTR>(vertexBufferSize);
        subresourceData.SlicePitch = static_cast<LONG_PTR>(vertexBufferSize);

        UpdateSubresources(commandList.Get(),
            buffer->d3d12Resource.Get(),
            uploadResource.Get(),
            0, 0, 1, &subresourceData);

        buffer->d3d12VertexBufferView.BufferLocation = buffer->d3d12Resource->GetGPUVirtualAddress();
        buffer->d3d12VertexBufferView.StrideInBytes = sizeof(Vertex);
        buffer->d3d12VertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
        buffer->vertexCount = vertices.size();

        resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(uploadResource));
    }

    return buffer;
}

Vertix::VertexBuffer* Vertix::VertexBuffer::CreateFullScreenRect(
    const GraphicsDevice *graphicsDevice,
    const GraphicsCommandList *graphicsCommandList,
    ResourceUploadHeap &resourceUploadHeap)
{
    const std::vector<Vertex> vertices
    {
        { .Position = {-1, -1, 0}, .TexCoord = {0,1} },
        { .Position = {-1, 1, 0}, .TexCoord = {0,0} },
        { .Position = {1, -1, 0}, .TexCoord = {1,1} },
        { .Position = {1, 1, 0}, .TexCoord = {1,0} }
    };

    return Create(
        vertices,
        graphicsDevice->GetD3D12Device(),
        graphicsCommandList->GetD3D12GraphicsCommandList(),
        resourceUploadHeap);
}
