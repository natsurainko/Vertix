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

    void Execute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) override;

private:
    Vertix::SwapChain* swapChain;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

#endif //VERTIX_LIGHTINGPASS_H
