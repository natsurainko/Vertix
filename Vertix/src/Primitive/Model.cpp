//
// Created by Natsurainko on 2026/1/13.
//

#include "Primitive/Model.h"

#include "Graphics/FrameCommandList.h"
#include "Graphics/GraphicsDevice.h"

void Vertix::Model::UploadToGPU(
    const GraphicsDevice* graphicsDevice,
    const GraphicsCommandList* graphicsCommandList,
    ResourceUploadHeap &resourceUploadHeap)
{
    const auto &device = graphicsDevice->GetD3D12Device();
    const auto &commandList = graphicsCommandList->GetD3D12GraphicsCommandList();

    for (auto &mesh: Meshes) {
        mesh.UploadToGPU(device, commandList, resourceUploadHeap);
    }
}

void Vertix::Model::Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) const {
    for (auto &mesh: Meshes) {
        mesh.Draw(commandList);
    }
}

void Vertix::Model::DrawInstanced(
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
    const UINT instanceCount) const
{
    for (auto &mesh: Meshes) {
        mesh.DrawInstanced(commandList, instanceCount);
    }
}
