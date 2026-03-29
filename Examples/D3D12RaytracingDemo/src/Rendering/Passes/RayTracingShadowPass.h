//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_RAYTRACINGPASS_H
#define VERTIX_RAYTRACINGPASS_H

#include "../RenderContext.h"
#include "Graphics/GraphicsDevice.h"
#include "Rendering/RenderPass.hpp"
#include "Rendering/UnorderedAccessView.h"

class RayTracingShadowPass : public Vertix::RenderPass<RenderContext> {
public:
    ~RayTracingShadowPass() override;

    void Initialize(
        Vertix::GraphicsDevice* device,
        RenderContext* context) override;

    void Execute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) override;
    void Resize(const Vertix::Vector2D<unsigned> &size) override;

private:
    Vertix::UnorderedAccessView* unorderedAccessView = nullptr;

    D3D12_CPU_DESCRIPTOR_HANDLE uavHandle{};
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle{};

    D3D12_GPU_DESCRIPTOR_HANDLE uavGpuHandle{};

    D3D12_RESOURCE_BARRIER srvBarrier{};

    uint32_t shaderTableEntrySize = 0;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> globalRootSig;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> localRootSig;
    Microsoft::WRL::ComPtr<ID3D12StateObject> rtStateObject;

    Microsoft::WRL::ComPtr<ID3D12Resource> stbResource;
};


#endif //VERTIX_RAYTRACINGPASS_H