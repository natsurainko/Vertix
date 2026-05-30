//
// Created by Natsurainko on 2026/1/13.
//

#include "Vertix/Primitive/Model.h"

#include "Vertix/Graphics/GraphicsDevice.h"

void Vertix::Model::UploadToGPU(
    ID3D12Device*              device,
    ID3D12GraphicsCommandList* commandList,
    ResourceUploadHeap &       resourceUploadHeap) {
    for (auto &mesh : Meshes) {
        mesh.UploadToGPU(device, commandList, resourceUploadHeap);
    }
}

#if VERTIX_D3D12_DEVICE_VERSION >= 5 && VERTIX_D3D12_COMMAND_LIST_VERSION >= 5
void Vertix::Model::UploadAccelerationStructureToGPU(
    D3D12Interface::Device*      device,
    D3D12Interface::CommandList* commandList) {
    for (auto &mesh : Meshes) {
        mesh.UploadAccelerationStructureToGPU(device, commandList);
    }
}
#endif

void Vertix::Model::Draw(ID3D12GraphicsCommandList* commandList) const {
    for (auto &mesh : Meshes) {
        mesh.Draw(commandList);
    }
}

void Vertix::Model::DrawInstanced(
    ID3D12GraphicsCommandList* commandList,
    const uint32_t             instanceCount) const {
    for (auto &mesh : Meshes) {
        mesh.DrawInstanced(commandList, instanceCount);
    }
}
