//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_RAYTRACINGPASS_H
#define VERTIX_RAYTRACINGPASS_H

#include <Vertix/Rendering/Pipeline/RenderPass.h>

#include "../RenderContext.h"

class RayTracingShadowPass : public Vertix::RenderPass<RenderContext> {
public:
    void Initialize(
        const Vertix::GraphicsDevice* device,
        const Vertix::PassInitializationContext &passContext,
        RenderContext* context) override;

    void Execute(ID3D12GraphicsCommandList5* commandList) override;

private:
    ID3D12DescriptorHeap* descriptorHeap = nullptr;

    const Vertix::RenderTextureView<Vertix::ShaderResource>* gPositionDepthSRV = nullptr;
    const Vertix::RenderTextureView<Vertix::ShaderResource>* gNormalRoughnessSRV= nullptr;
    const Vertix::RenderTextureView<Vertix::UnorderedAccess>* shadowMaskUAV = nullptr;

    uint32_t shaderTableEntrySize = 0;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> globalRootSig;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> localRootSig;
    Microsoft::WRL::ComPtr<ID3D12StateObject> rtStateObject;

    Microsoft::WRL::ComPtr<ID3D12Resource> stbResource;
};


#endif //VERTIX_RAYTRACINGPASS_H
