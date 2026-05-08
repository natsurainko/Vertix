//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_PASSINITIALIZATIONCONTEXT_H
#define VERTIX_PASSINITIALIZATIONCONTEXT_H

#include <assert.h>
#include <string>
#include <unordered_map>

#include "PassDeclarationBuilder.h"
#include "Vertix/Rendering/RenderResourceView.h"
#include "Vertix/Rendering/RenderResourceViewAllocator.hpp"

namespace Vertix {
    class PassInitializationContext {
    public:
        template<RenderResourceAccessor Accessor> requires SingleAccessor<Accessor>
        const RenderResourceView<Accessor>* GetView(const std::string& textureId) const {
            return GetExplicitView<Accessor>(PassDeclarationBuilder::ImplicitViewId(textureId, Accessor));
        }

        template<RenderResourceAccessor Accessor> requires SingleAccessor<Accessor>
        [[nodiscard]]
        const RenderResourceView<Accessor>* GetExplicitView(const std::string& viewId) const {
            if constexpr (Accessor == RenderTarget) {
                return rtvViews.at(viewId);
            } else if constexpr (Accessor == DepthStencil) {
                return dsvViews.at(viewId);
            } else if constexpr (Accessor == UnorderedAccess) {
                return uavViews.at(viewId);
            } else if constexpr (Accessor == ShaderResource) {
                return srvViews.at(viewId);
            }

            assert(false && "The specified texture does not exist or has not been declared.");
            return nullptr;
        }

        [[nodiscard]]
        const RenderResourceView<RenderTarget>** GetCurrentFrameRTV() const {
            return currentFrameRTV;
        }

        [[nodiscard]]
        ID3D12DescriptorHeap* GetShaderDescriptorHeap() const {
            return viewAllocator->GetShaderResourceDescriptorHeap()->GetDescriptorHeap().Get();
        }

    private:
        template <typename TContext>
        friend class RenderPipelineBuilder;

        std::unordered_map<std::string, const RenderResourceView<RenderTarget>*>    rtvViews;
        std::unordered_map<std::string, const RenderResourceView<DepthStencil>*>    dsvViews;
        std::unordered_map<std::string, const RenderResourceView<UnorderedAccess>*> uavViews;
        std::unordered_map<std::string, const RenderResourceView<ShaderResource>*>  srvViews;

        const RenderResourceView<RenderTarget>** currentFrameRTV = nullptr;
        const RenderResourceViewAllocator* viewAllocator = nullptr;
    };
}

#endif //VERTIX_PASSINITIALIZATIONCONTEXT_H
