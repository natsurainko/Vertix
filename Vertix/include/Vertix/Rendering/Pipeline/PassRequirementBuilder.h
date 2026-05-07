//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPASSBUILDER_H
#define VERTIX_RENDERPASSBUILDER_H

#include <string>
#include <vector>
#include <d3d12/d3d12.h>

#include "Vertix/Rendering/RenderTextureAccessor.h"
#include "Vertix/Rendering/RenderTextureView.h"

namespace Vertix {
    class PassRequirementBuilder {
    public:
        struct PassViewRequirement {
            std::string           viewId;
            RenderTextureAccessor viewAccessor;
            D3D12_RESOURCE_STATES requiredState;
        };

        struct PassRequirement {
            std::vector<PassViewRequirement> viewRequirements;
        };

        template<RenderTextureAccessor Accessor> requires SingleAccessor<Accessor>
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
