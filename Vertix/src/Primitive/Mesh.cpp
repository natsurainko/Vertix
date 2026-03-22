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
#include "Graphics/ResourceUploadHeap.hpp"

using Microsoft::WRL::ComPtr;

Vertix::Mesh::~Mesh() {
    delete VertexBuffer;
    delete IndexBuffer;
}

void Vertix::Mesh::UploadToGPU(
    const ComPtr<ID3D12Device> &device,
    const ComPtr<ID3D12GraphicsCommandList> &commandList,
    ResourceUploadHeap &resourceUploadHeap)
{
    VertexBuffer = VertexBuffer::Create(Vertices, device, commandList, resourceUploadHeap);
    IndexBuffer = IndexBuffer::Create(Indices, device, commandList, resourceUploadHeap);
}

void Vertix::Mesh::Draw(const ComPtr<ID3D12GraphicsCommandList> &commandList) const {
    commandList->IASetVertexBuffers(0, 1, &VertexBuffer->d3d12VertexBufferView);
    commandList->IASetIndexBuffer(&IndexBuffer->d3d12IndexBufferView);
    commandList->DrawIndexedInstanced(IndexBuffer->indexCount, 1, 0, 0, 0);
}

void Vertix::Mesh::DrawInstanced(
    const ComPtr<ID3D12GraphicsCommandList> &commandList,
    const UINT instanceCount) const
{
    commandList->IASetVertexBuffers(0, 1, &VertexBuffer->d3d12VertexBufferView);
    commandList->IASetIndexBuffer(&IndexBuffer->d3d12IndexBufferView);
    commandList->DrawIndexedInstanced(IndexBuffer->indexCount, instanceCount, 0, 0, 0);
}
