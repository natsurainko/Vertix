//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_PASSINITIALIZATIONCONTEXT_H
#define VERTIX_PASSINITIALIZATIONCONTEXT_H

#include <assert.h>
#include <string>
#include <unordered_map>

#include "Vertix/Rendering/RenderTextureView.h"
#include "Vertix/Rendering/RenderTextureViewAllocator.hpp"

namespace Vertix {
    class PassInitializationContext {
    public:
        template<RenderTextureAccessor Accessor> requires SingleAccessor<Accessor>
        [[nodiscard]]
        const RenderTextureView<Accessor>* GetTextureView(const std::string& id) const {
            if constexpr (Accessor == RenderTarget) {
                return rtvViews.at(id);
            } else if constexpr (Accessor == DepthStencil) {
                return dsvViews.at(id);
            } else if constexpr (Accessor == UnorderedAccess) {
                return uavViews.at(id);
            } else if constexpr (Accessor == ShaderResource) {
                return srvViews.at(id);
            }

            assert(false && "The specified texture does not exist or has not been declared.");
            return nullptr;
        }

        [[nodiscard]]
        const RenderTextureView<RenderTarget>** GetCurrentFrameRTV() const {
            return currentFrameRTV;
        }

        [[nodiscard]]
        ID3D12DescriptorHeap* GetShaderDescriptorHeap() const {
            return viewAllocator->GetShaderResourceDescriptorHeap()->GetDescriptorHeap().Get();
        }

    private:
        template <typename TContext>
        friend class RenderPipelineBuilder;

        std::unordered_map<std::string, const RenderTextureView<RenderTarget>*>    rtvViews;
        std::unordered_map<std::string, const RenderTextureView<DepthStencil>*>    dsvViews;
        std::unordered_map<std::string, const RenderTextureView<UnorderedAccess>*> uavViews;
        std::unordered_map<std::string, const RenderTextureView<ShaderResource>*>  srvViews;

        const RenderTextureView<RenderTarget>** currentFrameRTV = nullptr;
        const RenderTextureViewAllocator* viewAllocator = nullptr;
    };
}

#endif //VERTIX_PASSINITIALIZATIONCONTEXT_H
