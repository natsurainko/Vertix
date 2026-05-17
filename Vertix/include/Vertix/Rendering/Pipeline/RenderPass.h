//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPASS_H
#define VERTIX_RENDERPASS_H

#include <concepts>
#include <d3d12/d3d12.h>

namespace Vertix {
    class RenderPass {
    public:
        virtual ~RenderPass() = default;
        virtual void Initialize(ID3D12Device10* device) = 0;
        virtual void Execute(ID3D12GraphicsCommandList5* commandList) = 0;
    };

    template <typename TRenderPass>
    concept RenderPassType = std::derived_from<TRenderPass, RenderPass>;
}

#endif //VERTIX_RENDERPASS_H
