//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPASS_H
#define VERTIX_RENDERPASS_H

#include <d3d12/d3d12.h>

#include "PassInitializationContext.h"

namespace Vertix {
    class GraphicsDevice;

    template <typename TContext>
    class RenderPass {
    public:
        virtual ~RenderPass() = default;

        virtual void Initialize(
            const GraphicsDevice* device,
            const PassInitializationContext& passContext,
            TContext* context) = 0;

        virtual void Execute(ID3D12GraphicsCommandList5* commandList) = 0;

    protected:
        TContext* renderContext  = nullptr;
    };

    template <typename TRenderPass, typename TContext>
    concept RenderPassType = std::derived_from<TRenderPass, RenderPass<TContext>>;
}

#endif //VERTIX_RENDERPASS_H
