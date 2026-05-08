//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPASSBUILDER_H
#define VERTIX_RENDERPASSBUILDER_H

#include <string>
#include <vector>
#include <optional>
#include <stdexcept>
#include <variant>
#include <d3d12/d3d12.h>

#include "Vertix/Rendering/RenderResourceAccessor.h"
#include "Vertix/Rendering/RenderResourceView.h"

namespace Vertix {
    class PassDeclarationBuilder {
    public:
        enum PassDataflowDirection {
            Read,
            Write
        };

        struct PassDataflowImplicitDeclaration {
            std::string            textureId;
            RenderResourceAccessor accessor;
            std::optional<D3D12_RESOURCE_STATES> explicitState;

        private:
            template<typename>
            friend class RenderPipelineBuilder;

            [[nodiscard]]
            D3D12_RESOURCE_STATES DerivativeState(const PassDataflowDirection direction) const {
                if (explicitState.has_value()) {
                    return explicitState.value();
                }

                if (direction == Read) {
                    switch (accessor) {
                        case RenderTarget:
                            assert(false && "Not supported accessor");
                            break;
                        case DepthStencil:
                            return D3D12_RESOURCE_STATE_DEPTH_READ;
                        case UnorderedAccess:
                            return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                        case ShaderResource:
                            return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
                        default:
                            assert(false && "Not supported accessor");
                            break;
                    }
                }

                if (direction == Write) {
                    switch (accessor) {
                        case RenderTarget:
                            return D3D12_RESOURCE_STATE_RENDER_TARGET;
                        case DepthStencil:
                            return D3D12_RESOURCE_STATE_DEPTH_WRITE;
                        case UnorderedAccess:
                            return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                        case ShaderResource:
                            assert(false && "Not supported accessor");
                            break;
                        default:
                            assert(false && "Not supported accessor");
                            break;
                    }
                }

                return D3D12_RESOURCE_STATE_COMMON;
            }
        };

        struct PassDataflowExplicitDeclaration {
            std::string           viewId;
            D3D12_RESOURCE_STATES explicitState;
        };

        struct PassDataflowDeclaration {
            PassDataflowDirection  direction;

            std::variant<
                PassDataflowImplicitDeclaration,
                PassDataflowExplicitDeclaration
            > declaration;
        };

        struct PassDeclaration {
            std::vector<PassDataflowDeclaration> dataflows;
        } declaration;

        template<RenderResourceAccessor A> requires SingleAccessor<A>
        PassDeclarationBuilder& DeclareRead(const std::string& resourceId) {
            declaration.dataflows.emplace_back(Read, PassDataflowImplicitDeclaration {
                .textureId = resourceId,
                .accessor = A,
                .explicitState = std::nullopt
            });

            return *this;
        }

        template<RenderResourceAccessor A> requires SingleAccessor<A>
        PassDeclarationBuilder& DeclareWrite(const std::string& resourceId) {
            declaration.dataflows.emplace_back(Write, PassDataflowImplicitDeclaration {
                .textureId = resourceId,
                .accessor = A,
                .explicitState = std::nullopt
            });

            return *this;
        }

        template<RenderResourceAccessor A> requires SingleAccessor<A>
        PassDeclarationBuilder& DeclareReadExplicit(
            const std::string& viewId,
            const D3D12_RESOURCE_STATES explicitState)
        {
            declaration.dataflows.emplace_back(Read, PassDataflowExplicitDeclaration {
                .viewId = viewId,
                .explicitState = explicitState
            });

            return *this;
        }

        template<RenderResourceAccessor A> requires SingleAccessor<A>
        PassDeclarationBuilder& DeclareWriteExplicit(
            const std::string& viewId,
            const D3D12_RESOURCE_STATES explicitState)
        {
            declaration.dataflows.emplace_back(Write, PassDataflowExplicitDeclaration {
                .viewId = viewId,
                .explicitState = explicitState
            });

            return *this;
        }

        [[nodiscard]]
        PassDeclaration Build() noexcept {
            return std::move(declaration);
        }

        static std::string ImplicitViewId(const std::string& textureId, RenderResourceAccessor accessor) {
            switch (accessor) {
                case RenderTarget:    return textureId + ":Implicit:RenderTarget";
                case DepthStencil:    return textureId + ":Implicit:DepthStencil";
                case UnorderedAccess: return textureId + ":Implicit:UnorderedAccess";
                case ShaderResource:  return textureId + ":Implicit:ShaderResource";
                default: throw std::invalid_argument("Not supported accessor");
            }
        }
    };
}

#endif //VERTIX_RENDERPASSBUILDER_H
