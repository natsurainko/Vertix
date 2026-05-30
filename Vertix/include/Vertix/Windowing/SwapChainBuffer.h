//
// Created by Natsurainko on 2026/5/28.
//

#pragma once

#include "Vertix/Rendering/RenderResource.h"

namespace Vertix {
    class SwapChain;

    class SwapChainBuffer : public RenderResource {
        const SwapChain* swapChain;
        uint32_t         frameIndex;

    public:
        VERTIX_API explicit SwapChainBuffer(
            const SwapChain* swapChain,
            uint32_t         frameIndex);

        VERTIX_API void RefreshBufferResource(D3D12_RESOURCE_STATES resourceStates);

        static Microsoft::WRL::ComPtr<ID3D12Resource> GetBufferResource(
            const SwapChain* swapChain,
            uint32_t         frameIndex);
    };
}
