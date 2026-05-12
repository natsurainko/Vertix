//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPASSBUILDER_H
#define VERTIX_RENDERPASSBUILDER_H

#include <assert.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include <d3d12/d3d12.h>

#include "Vertix/Mixin/IPassBinding.h"
#include "Vertix/Rendering/RenderResourceAccessor.h"
#include "Vertix/Rendering/RenderResourceView.h"

namespace Vertix {
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
    struct PassDataflowSwapChainDeclaration {};
    struct PassDataflowDeclaration {
        PassDataflowDirection direction;
        std::unique_ptr<IPassBinding> dataflowBinding;

        std::variant<
            PassDataflowImplicitDeclaration,
            PassDataflowExplicitDeclaration,
            PassDataflowSwapChainDeclaration
        > declaration;
    };
    struct PassDeclaration {
        std::vector<PassDataflowDeclaration> dataflows;
    };

    template<typename TPass>
    class PassDeclarationBuilder {
        template<typename TMember>
        struct PassBinding : IPassBinding {
            explicit PassBinding(TMember TPass::* member) :member(member) {}
            TMember TPass::* member;

            void Inject(void* pass, void* resource) override {
                auto typedPass = static_cast<TPass*>(pass);
                typedPass->*member = static_cast<TMember>(resource);
            }
        };
    public:
        PassDeclaration declaration;

        template<typename TMember> requires SingleAccessor<AccessorOf<TMember>>
        PassDeclarationBuilder& DeclareWrite(
            const std::string& resourceId,
            TMember TPass::* field)
        {
            constexpr RenderResourceAccessor accessor = AccessorOf<TMember>;

            declaration.dataflows.emplace_back(Write,
                std::make_unique<PassBinding<TMember>>(field),
                PassDataflowImplicitDeclaration {
                    .textureId     = resourceId,
                    .accessor      = accessor,
                    .explicitState = std::nullopt
                });

            return *this;
        }

        template<typename TMember> requires SingleAccessor<AccessorOf<TMember>>
        PassDeclarationBuilder& DeclareRead(
            const std::string& resourceId,
            TMember TPass::* field)
        {
            constexpr RenderResourceAccessor accessor = AccessorOf<TMember>;

            declaration.dataflows.emplace_back(Read,
                std::make_unique<PassBinding<TMember>>(field),
                PassDataflowImplicitDeclaration {
                    .textureId     = resourceId,
                    .accessor      = accessor,
                    .explicitState = std::nullopt
                });

            return *this;
        }

        template<typename TMember> requires SingleAccessor<AccessorOf<TMember>>
        PassDeclarationBuilder& DeclareReadExplicit(
            const std::string& viewId,
            TMember TPass::* field,
            const D3D12_RESOURCE_STATES explicitState)
        {
            declaration.dataflows.emplace_back(Read,
                std::make_unique<PassBinding<TMember>>(field),
                PassDataflowExplicitDeclaration {
                    .viewId        = viewId,
                    .explicitState = explicitState
                });

            return *this;
        }

        template<typename TMember> requires SingleAccessor<AccessorOf<TMember>>
        PassDeclarationBuilder& DeclareWriteExplicit(
            const std::string& viewId,
            TMember TPass::* field,
            const D3D12_RESOURCE_STATES explicitState)
        {
            declaration.dataflows.emplace_back(Write,
                std::make_unique<PassBinding<TMember>>(field),
                PassDataflowExplicitDeclaration {
                    .viewId        = viewId,
                    .explicitState = explicitState
                });

            return *this;
        }

        PassDeclarationBuilder& DeclareSwapChainWrite(const RenderResourceView<RenderTarget>** TPass::* field) {
            declaration.dataflows.emplace_back(
                Write,
                std::make_unique<PassBinding<const RenderResourceView<RenderTarget>**>>(field),
                PassDataflowSwapChainDeclaration{}
            );
            return *this;
        }

        [[nodiscard]]
        PassDeclaration Build() noexcept {
            return std::move(declaration);
        }
    };

    static std::string PassImplicitViewId(
        const std::string& textureId,
        const RenderResourceAccessor accessor)
    {
        switch (accessor) {
            case RenderTarget:    return textureId + ":Implicit:RenderTarget";
            case DepthStencil:    return textureId + ":Implicit:DepthStencil";
            case UnorderedAccess: return textureId + ":Implicit:UnorderedAccess";
            case ShaderResource:  return textureId + ":Implicit:ShaderResource";
            default: throw std::invalid_argument("Not supported accessor");
        }
    }
}

#endif //VERTIX_RENDERPASSBUILDER_H
