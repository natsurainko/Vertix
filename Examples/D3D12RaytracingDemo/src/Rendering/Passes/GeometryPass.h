//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_GEOMETRYPASS_H
#define VERTIX_GEOMETRYPASS_H

#include "../RenderContext.h"
#include "Vertix/Rendering/RenderPass.hpp"

class GeometryPass : public Vertix::RenderPass<RenderContext> {
public:
    void Initialize(
        Vertix::GraphicsDevice* device,
        RenderContext* context) override;

    void Execute(ID3D12GraphicsCommandList5* commandList) override;
    void Resize(const Vertix::Vector2D<unsigned> &size) override;

private:
    std::unique_ptr<Vertix::RenderTexture<Vertix::DrawColorSampleAccessor>> gPositionDepthTexture;
    std::unique_ptr<Vertix::RenderTexture<Vertix::DrawColorSampleAccessor>> gNormalRoughnessTexture;
    std::unique_ptr<Vertix::RenderTexture<Vertix::DepthStencil>> gDepthTexture;

    Vertix::RenderTextureView<Vertix::RenderTarget> gPositionDepthRTV;
    Vertix::RenderTextureView<Vertix::RenderTarget> gNormalRoughnessRTV;
    Vertix::RenderTextureView<Vertix::DepthStencil> gDepthDSV;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

#endif //VERTIX_GEOMETRYPASS_H
