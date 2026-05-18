//
// Created by Natsurainko on 2026/3/29.
//

#ifndef VERTIX_LIGHTINGPASS_H
#define VERTIX_LIGHTINGPASS_H

#include <Vertix/Rendering/RenderResourceView.h>
#include <Vertix/Rendering/Pipeline/RenderPass.h>

#include "../RenderContext.h"

class LightingPass : public Vertix::RenderPass {
public:
    explicit LightingPass(Vertix::SwapChain* swapChain, RenderContext* renderContext)
    : swapChain(swapChain), renderContext(renderContext) {}

    void Initialize(ID3D12Device10* device) override;
    void Execute(ID3D12GraphicsCommandList5* commandList) override;

    const Vertix::RenderResourceView<Vertix::RenderResourceViewType::ShaderResource>* shadowMaskSRV = nullptr;
    const Vertix::RenderResourceView<Vertix::RenderResourceViewType::RenderTarget>* currentFrameRTV = nullptr;

private:
    Vertix::SwapChain*    swapChain;
    RenderContext*        renderContext;
    ID3D12DescriptorHeap* descriptorHeap = nullptr;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

#endif //VERTIX_LIGHTINGPASS_H
