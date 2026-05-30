//
// Created by Natsurainko on 2026/5/7.
//

#pragma once

#include <concepts>

#include "Vertix/D3D12Interface.h"

namespace Vertix {
    class RenderPass {
    public:
        virtual      ~RenderPass() = default;
        virtual void Initialize(D3D12Interface::Device* device) = 0;
        virtual void Execute(D3D12Interface::CommandList* commandList) = 0;
    };

    template <typename TRenderPass>
    concept RenderPassType = std::derived_from<TRenderPass, RenderPass>;
}
