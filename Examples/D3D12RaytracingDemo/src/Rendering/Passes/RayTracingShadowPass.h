//
// Created by Natsurainko on 2026/3/28.
//

#ifndef VERTIX_RAYTRACINGPASS_H
#define VERTIX_RAYTRACINGPASS_H

#include <Vertix/Rendering/Pipeline/RenderPass.h>

#include "../RenderContext.h"

class RayTracingShadowPass : public Vertix::RenderPass {
public:
    explicit RayTracingShadowPass(RenderContext* renderContext) : renderContext(renderContext) {}

    void Initialize(ID3D12Device10* device) override;
    void Execute(ID3D12GraphicsCommandList5* commandList) override;

    const Vertix::RenderResourceView<Vertix::RenderResourceViewType::ShaderResource>* gPositionDepthSRV = nullptr;
    const Vertix::RenderResourceView<Vertix::RenderResourceViewType::ShaderResource>* gNormalRoughnessSRV= nullptr;
    const Vertix::RenderResourceView<Vertix::RenderResourceViewType::UnorderedAccess>* shadowMaskUAV = nullptr;

private:
    RenderContext*        renderContext;
    ID3D12DescriptorHeap* descriptorHeap = nullptr;

    uint32_t shaderTableEntrySize = 0;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> globalRootSig;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> localRootSig;
    Microsoft::WRL::ComPtr<ID3D12StateObject> rtStateObject;

    Microsoft::WRL::ComPtr<ID3D12Resource> stbResource;
};


#endif //VERTIX_RAYTRACINGPASS_H
