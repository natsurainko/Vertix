//
// Created by Natsurainko on 2026/3/29.
//

#ifndef VERTIX_LIGHTINGPASS_H
#define VERTIX_LIGHTINGPASS_H

#include "../RenderContext.h"
#include "Vertix/Rendering/RenderPass.hpp"

class LightingPass : public Vertix::RenderPass<RenderContext> {
public:
    explicit LightingPass(Vertix::SwapChain* swapChain);

    void Initialize(
        Vertix::GraphicsDevice* device,
        RenderContext* context) override;

    void Execute(ID3D12GraphicsCommandList5* commandList) override;
    void Resize(const Vertix::Vector2D<unsigned> &size) override;

private:
    Vertix::SwapChain* swapChain;

    Vertix::RenderTextureView<Vertix::ShaderResource> shadowMaskSRV;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

#endif //VERTIX_LIGHTINGPASS_H
