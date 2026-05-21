//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_RAYTRACINGPASS_H
#define VERTIX_RAYTRACINGPASS_H

#include <Vertix/Graphics/DescriptorView.h>
#include <Vertix/Rendering/Pipeline/RenderPass.h>

#include "../RenderContext.h"

class RayTracingShadowPass : public Vertix::RenderPass {
public:
    explicit RayTracingShadowPass(RenderContext* renderContext) : renderContext(renderContext) {}

    void Initialize(ID3D12Device10* device) override;
    void Execute(ID3D12GraphicsCommandList5* commandList) override;

    Vertix::DescriptorView<Vertix::RenderResourceUsage::PixelShaderResource> gPositionDepthSRV;
    Vertix::DescriptorView<Vertix::RenderResourceUsage::PixelShaderResource> gNormalRoughnessSRV;
    Vertix::DescriptorView<Vertix::RenderResourceUsage::UnorderedAccess> shadowMaskUAV;

    D3D12_GPU_VIRTUAL_ADDRESS lightConstantsAddress = {};

private:
    RenderContext* renderContext;

    uint32_t shaderTableEntrySize = 0;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> globalRootSig;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> localRootSig;
    Microsoft::WRL::ComPtr<ID3D12StateObject> rtStateObject;

    Microsoft::WRL::ComPtr<ID3D12Resource> stbResource;
};


#endif //VERTIX_RAYTRACINGPASS_H
