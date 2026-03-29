//
// Created by Natsurainko on 2026/1/13.
//

#include "Primitive/Model.h"

#include "Graphics/FrameCommandList.h"
#include "Graphics/GraphicsDevice.h"

void Vertix::Model::UploadToGPU(
    const Microsoft::WRL::ComPtr<ID3D12Device> &device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,
    ResourceUploadHeap &resourceUploadHeap)
{
    for (auto &mesh: Meshes) {
        mesh.UploadToGPU(device, commandList, resourceUploadHeap);
    }
}

void Vertix::Model::UploadBLASToGPU(
    const Microsoft::WRL::ComPtr<ID3D12Device5> &device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> &commandList)
{
    for (auto &mesh: Meshes) {
        mesh.UploadBLASToGPU(device, commandList);
    }
}

void Vertix::Model::Draw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList) const {
    for (auto &mesh: Meshes) {
        mesh.Draw(commandList);
    }
}

void Vertix::Model::DrawInstanced(
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList,
    const UINT instanceCount) const
{
    for (auto &mesh: Meshes) {
        mesh.DrawInstanced(commandList, instanceCount);
    }
}
