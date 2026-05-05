//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_RAYTRACINGPASS_H
#define VERTIX_RAYTRACINGPASS_H

#include "../RenderContext.h"
#include "Vertix/Graphics/GraphicsDevice.h"
#include "Vertix/Rendering/RenderPass.hpp"

class RayTracingShadowPass : public Vertix::RenderPass<RenderContext> {
public:
    void Initialize(
        Vertix::GraphicsDevice* device,
        RenderContext* context) override;

    void Execute(ID3D12GraphicsCommandList5* commandList) override;
    void Resize(const Vertix::Vector2D<unsigned> &size) override;

private:
    std::unique_ptr<Vertix::RenderTexture<Vertix::UnorderedAccessSampleAccessor>> shadowMaskTexture;

    Vertix::RenderTextureView<Vertix::ShaderResource> gPositionDepthSRV;
    Vertix::RenderTextureView<Vertix::ShaderResource> gNormalRoughnessSRV;
    Vertix::RenderTextureView<Vertix::UnorderedAccess> shadowMaskUAV;

    uint32_t shaderTableEntrySize = 0;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> globalRootSig;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> localRootSig;
    Microsoft::WRL::ComPtr<ID3D12StateObject> rtStateObject;

    Microsoft::WRL::ComPtr<ID3D12Resource> stbResource;
};


#endif //VERTIX_RAYTRACINGPASS_H
