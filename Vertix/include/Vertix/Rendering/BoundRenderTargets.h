//
// Created by Natsurainko on 2026/5/6.
//

#ifndef VERTIX_BOUNDRENDERTARGETS_H
#define VERTIX_BOUNDRENDERTARGETS_H

#include <intsafe.h>
#include <d3d12/d3d12.h>

#include "RenderTextureView.h"

namespace Vertix {
    template <UINT RTCount, bool HasDS = false>
    struct BoundRenderTargets {
        template <typename... Args> requires (HasDS && sizeof...(Args) == RTCount)
        explicit BoundRenderTargets(RenderTextureView<DepthStencil>* dsv, Args... rtvs) : RTVs{rtvs...}, DSV(dsv) {}

        template <typename... Args> requires (!HasDS && sizeof...(Args) == RTCount)
        explicit BoundRenderTargets(Args... rtvs) : RTVs{rtvs...} {}

        void ClearRenderTargetViews(ID3D12GraphicsCommandList* commandList, const float clearColor[4]) const {
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (commandList->ClearRenderTargetView(RTVs[I]->GetHandle(), clearColor, 0, nullptr), ...);
            }(std::make_index_sequence<RTCount>{});
        }

        void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList, const float depth = 1.0f) const requires HasDS { DSV->ClearDepth(commandList, depth); }
        void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList, const UINT8 stencil = 0) const requires HasDS { DSV->ClearStencil(commandList, stencil); }
        void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList, const float depth = 1.0f, const UINT8 stencil = 0)  const requires HasDS { DSV->ClearDepthStencil(commandList, depth, stencil); }

        void SetRenderTargets(ID3D12GraphicsCommandList* commandList) {
            const auto rtvHandles = [&]<std::size_t... I>(std::index_sequence<I...>) {
                return std::array<D3D12_CPU_DESCRIPTOR_HANDLE, RTCount>{
                    RTVs[I]->GetHandle()...
                };
            }(std::make_index_sequence<RTCount>{});

            if constexpr (HasDS) {
                commandList->OMSetRenderTargets(RTCount, rtvHandles.data(), FALSE, DSV->GetHandleAddress());
            } else {
                commandList->OMSetRenderTargets(RTCount, rtvHandles.data(), FALSE, nullptr);
            }
        }
    private:
        RenderTextureView<RenderTarget>* RTVs[RTCount] = {};
        std::conditional_t<HasDS, RenderTextureView<DepthStencil>*, std::monostate > DSV = nullptr;
    };


}

#endif //VERTIX_BOUNDRENDERTARGETS_H
