//
// Created by Natsurainko on 2026/3/29.
//

#ifndef VERTIX_LIGHTINGPASS_H
#define VERTIX_LIGHTINGPASS_H

#include <Vertix/Graphics/DescriptorView.h>
#include <Vertix/Rendering/Pipeline/RenderPass.h>

#include "../RenderContext.h"

class LightingPass : public Vertix::RenderPass {
public:
    explicit LightingPass(RenderContext* renderContext) : renderContext(renderContext) {}

    void Initialize(ID3D12Device10* device) override;
    void Execute(ID3D12GraphicsCommandList5* commandList) override;

    Vertix::DescriptorView<Vertix::RenderResourceUsage::PixelShaderResource> shadowMaskSRV;
    Vertix::DescriptorView<Vertix::RenderResourceUsage::RenderTarget> currentFrameRTV;

private:
    RenderContext* renderContext;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

#endif //VERTIX_LIGHTINGPASS_H
