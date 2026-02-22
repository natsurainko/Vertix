//
// Created by Natsurainko on 2026/1/13.
//

#include "Primitive/Mesh.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>
#include <d3d12/d3dx12_resource_helpers.h>

#include "Exceptions/HResultException.h"
#include "Graphics/FrameCommandList.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ResourceUploadHeap.h"

using Microsoft::WRL::ComPtr;

Vertix::Mesh::~Mesh() {
    delete VertexBuffer;
    delete IndexBuffer;
}

void Vertix::Mesh::UploadToGPU(const ComPtr<ID3D12Device10> &device,
                               const ComPtr<ID3D12GraphicsCommandList5> &commandList,
                               ResourceUploadHeap &resourceUploadHeap) {
    const auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    ComPtr<ID3D12Resource> vertexBufferUpload;
    ComPtr<ID3D12Resource> indexBufferUpload;

    VertexBuffer = new Vertix::VertexBuffer();
    IndexBuffer = new Vertix::IndexBuffer();

    {
        const UINT64 vertexBufferSize = Vertices.size() * sizeof(Vertex);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

        ThrowIfFailed(device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&(VertexBuffer->d3d12Resource))));

        const CD3DX12_RESOURCE_BARRIER commonToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
            VertexBuffer->d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST);
        commandList->ResourceBarrier(1, &commonToCopyDest);

        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexBufferUpload)));

        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = Vertices.data();
        subresourceData.RowPitch = static_cast<LONG_PTR>(vertexBufferSize);
        subresourceData.SlicePitch = static_cast<LONG_PTR>(vertexBufferSize);

        UpdateSubresources(commandList.Get(),
            VertexBuffer->d3d12Resource.Get(),
            vertexBufferUpload.Get(),
            0, 0, 1, &subresourceData);

        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            VertexBuffer->d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        commandList->ResourceBarrier(1, &barrier);

        VertexBuffer->d3d12VertexBufferView.BufferLocation = VertexBuffer->d3d12Resource->GetGPUVirtualAddress();
        VertexBuffer->d3d12VertexBufferView.StrideInBytes = sizeof(Vertex);
        VertexBuffer->d3d12VertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
        VertexBuffer->vertexCount = Vertices.size();

        resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(vertexBufferUpload));
    }

    {
        const UINT64 indexBufferSize = Indices.size() * sizeof(UINT32);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

        ThrowIfFailed(device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&(IndexBuffer->d3d12Resource))));

        const CD3DX12_RESOURCE_BARRIER commonToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
            IndexBuffer->d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST);
        commandList->ResourceBarrier(1, &commonToCopyDest);

        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&indexBufferUpload)));

        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData = Indices.data();
        subresourceData.RowPitch = static_cast<LONG_PTR>(indexBufferSize);
        subresourceData.SlicePitch = static_cast<LONG_PTR>(indexBufferSize);

        UpdateSubresources(commandList.Get(),
            IndexBuffer->d3d12Resource.Get(),
            indexBufferUpload.Get(),
            0, 0, 1, &subresourceData);

        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            IndexBuffer->d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_INDEX_BUFFER);
        commandList->ResourceBarrier(1, &barrier);

        IndexBuffer->d3d12IndexBufferView.BufferLocation = IndexBuffer->d3d12Resource->GetGPUVirtualAddress();
        IndexBuffer->d3d12IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
        IndexBuffer->d3d12IndexBufferView.SizeInBytes = static_cast<UINT>(indexBufferSize);
        IndexBuffer->indexCount = Indices.size();

        resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(indexBufferUpload));
    }
}

void Vertix::Mesh::Draw(const ComPtr<ID3D12GraphicsCommandList5> &commandList) const {
    commandList->IASetVertexBuffers(0, 1, &VertexBuffer->d3d12VertexBufferView);
    commandList->IASetIndexBuffer(&IndexBuffer->d3d12IndexBufferView);
    commandList->DrawIndexedInstanced(IndexBuffer->indexCount, 1, 0, 0, 0);
}
