//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_GEOMETRYPASS_H
#define VERTIX_GEOMETRYPASS_H

#include <Vertix/Graphics/DescriptorView.h>
#include <Vertix/Rendering/Pipeline/RenderPass.h>

#include "../RenderContext.h"

class GeometryPass : public Vertix::RenderPass {
public:
    explicit GeometryPass(RenderContext* renderContext) : renderContext(renderContext) {}

    void Initialize(ID3D12Device10* device) override;
    void Execute(ID3D12GraphicsCommandList5* commandList) override;

    Vertix::DescriptorView<Vertix::RenderResourceUsage::RenderTarget> gPositionDepthRTV;
    Vertix::DescriptorView<Vertix::RenderResourceUsage::RenderTarget> gNormalRoughnessRTV;
    Vertix::DescriptorView<Vertix::RenderResourceUsage::DepthWrite> gDepthDSV;

    D3D12_GPU_VIRTUAL_ADDRESS frameConstants = {};

private:
    RenderContext* renderContext;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
};

#endif //VERTIX_GEOMETRYPASS_H
