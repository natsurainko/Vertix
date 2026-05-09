//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_GEOMETRYPASS_H
#define VERTIX_GEOMETRYPASS_H

#include <Vertix/Rendering/Pipeline/RenderPass.h>

#include "../RenderContext.h"

class GeometryPass : public Vertix::RenderPass<RenderContext> {
public:
    void Initialize(ID3D12Device10* device) override;
    void Execute(ID3D12GraphicsCommandList5* commandList) override;

    const Vertix::RenderResourceView<Vertix::RenderTarget>* gPositionDepthRTV = nullptr;
    const Vertix::RenderResourceView<Vertix::RenderTarget>* gNormalRoughnessRTV = nullptr;
    const Vertix::RenderResourceView<Vertix::DepthStencil>* gDepthDSV = nullptr;

private:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

#endif //VERTIX_GEOMETRYPASS_H
