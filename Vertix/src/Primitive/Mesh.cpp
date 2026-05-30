//
// Created by Natsurainko on 2026/1/13.
//

#include "Vertix/Primitive/Mesh.h"

#include "Vertix/Graphics/ResourceUploadHeap.hpp"
#include "Vertix/Rendering/Buffers/AccelerationStructure.h"
#include "Vertix/Rendering/Buffers/IndexBuffer.h"
#include "Vertix/Rendering/Buffers/VertexBuffer.h"

void Vertix::Mesh::UploadToGPU(
    ID3D12Device*              device,
    ID3D12GraphicsCommandList* commandList,
    ResourceUploadHeap &       resourceUploadHeap) {
    assert(!VertexBuffer);
    assert(!IndexBuffer);
    VertexBuffer = VertexBuffer::Create(Vertices, device, commandList, resourceUploadHeap);
    IndexBuffer  = IndexBuffer::Create(Indices, device, commandList, resourceUploadHeap);
}

#if VERTIX_D3D12_DEVICE_VERSION >= 5 && VERTIX_D3D12_COMMAND_LIST_VERSION >= 5
void Vertix::Mesh::UploadAccelerationStructureToGPU(
    D3D12Interface::Device*      device,
    D3D12Interface::CommandList* commandList) {
    assert(VertexBuffer);
    assert(IndexBuffer);
    assert(!AccelerationStructure);
    AccelerationStructure = AccelerationStructure::CreateBottomLevelAccelerationStructure(
        device,
        commandList,
        VertexBuffer.get(),
        IndexBuffer.get(),
        DXGI_FORMAT_R32G32B32_FLOAT
    );
}
#endif

void Vertix::Mesh::Draw(ID3D12GraphicsCommandList* commandList) const {
    commandList->IASetVertexBuffers(0, 1, &VertexBuffer->GetBufferView());
    commandList->IASetIndexBuffer(&IndexBuffer->GetBufferView());
    commandList->DrawIndexedInstanced(IndexBuffer->GetIndexCount(), 1, 0, 0, 0);
}

void Vertix::Mesh::DrawInstanced(
    ID3D12GraphicsCommandList* commandList,
    const uint32_t             instanceCount) const {
    commandList->IASetVertexBuffers(0, 1, &VertexBuffer->GetBufferView());
    commandList->IASetIndexBuffer(&IndexBuffer->GetBufferView());
    commandList->DrawIndexedInstanced(IndexBuffer->GetIndexCount(), instanceCount, 0, 0, 0);
}
