//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_GEOMETRYPASS_H
#define VERTIX_GEOMETRYPASS_H

#include "../RenderContext.h"
#include "Vertix/Rendering/RenderPass.hpp"

class GeometryPass : public Vertix::RenderPass<RenderContext> {
public:
    ~GeometryPass() override;

    void Initialize(
        Vertix::GraphicsDevice* device,
        RenderContext* context) override;

    void Execute(ID3D12GraphicsCommandList5* commandList) override;
    void Resize(const Vertix::Vector2D<unsigned> &size) override;

private:
    const Vertix::RenderTextureRenderTargetView* gPositionDepthRTV = nullptr;
    const Vertix::RenderTextureRenderTargetView* gNormalRoughnessRTV = nullptr;
    const Vertix::RenderTextureDepthStencilView* gDepthDSV = nullptr;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

#endif //VERTIX_GEOMETRYPASS_H
