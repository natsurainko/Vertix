//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPASSBUILDER_H
#define VERTIX_RENDERPASSBUILDER_H

#include <string>
#include <vector>
#include <d3d12/d3d12.h>

#include "Vertix/Rendering/RenderResourceAccessor.h"
#include "Vertix/Rendering/RenderResourceView.h"

namespace Vertix {
    class PassRequirementBuilder {
    public:
        struct PassViewRequirement {
            std::string           viewId;
            RenderResourceAccessor viewAccessor;
            D3D12_RESOURCE_STATES requiredState;
        };

        struct PassRequirement {
            std::vector<PassViewRequirement> viewRequirements;
        };

        template<RenderResourceAccessor Accessor> requires SingleAccessor<Accessor>
        void RequireView(const std::string& viewId) {
            if constexpr (Accessor == RenderTarget) {
                passRequirement.viewRequirements.emplace_back(PassViewRequirement{ viewId, Accessor, D3D12_RESOURCE_STATE_RENDER_TARGET });
            } else if constexpr (Accessor == DepthStencil) {
                passRequirement.viewRequirements.emplace_back(PassViewRequirement{ viewId, Accessor, D3D12_RESOURCE_STATE_DEPTH_WRITE });
            } else if constexpr (Accessor == UnorderedAccess) {
                passRequirement.viewRequirements.emplace_back(PassViewRequirement{ viewId, Accessor, D3D12_RESOURCE_STATE_UNORDERED_ACCESS });
            } else if constexpr (Accessor == ShaderResource) {
                passRequirement.viewRequirements.emplace_back(PassViewRequirement{ viewId, Accessor, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE });
            }
        }

        template<RenderResourceAccessor Accessor> requires SingleAccessor<Accessor>
        void RequireView(
            const std::string& viewId,
            const D3D12_RESOURCE_STATES requiredState)
        {
            passRequirement.viewRequirements.emplace_back(PassViewRequirement{ viewId, Accessor, requiredState });
        }

        [[nodiscard]]
        PassRequirement Build() noexcept {
            return std::move(passRequirement);
        }

    private:
        PassRequirement passRequirement {};
    };
}

#endif //VERTIX_RENDERPASSBUILDER_H
