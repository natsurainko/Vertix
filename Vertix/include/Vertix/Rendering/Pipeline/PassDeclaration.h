//
// Created by Natsurainko on 2026/5/14.
//

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>

#include "Vertix/Graphics/Descriptor/DescriptorHandle.h"
#include "Vertix/Graphics/Descriptor/DescriptorView.h"
#include "Vertix/Mixin/IPassBinding.h"
#include "Vertix/Rendering/RenderResource.h"
#include "Vertix/Rendering/RenderResourceUsage.h"
#include "Vertix/Rendering/Pipeline/RenderPass.h"

namespace Vertix {
    template <typename>
    class StructuredBuffer;

    struct PassResourceDeclaration {
        enum class PassResourceUsingMethod {
            Resource,
            View,
            GPUAddress
        } method;

        std::string                   resourceName;
        RenderResourceUsage           resourceUsage;
        std::shared_ptr<IPassBinding> resourceBinding;
    };

    struct PassDeclaration {
        std::vector<PassResourceDeclaration>         Resources;
        std::vector<std::type_index>                 PassDependencies;
        std::function<std::unique_ptr<RenderPass>()> factory;
        std::type_index                              passType      = typeid(void);
        bool                                         hasSideEffect = false;
    };

    template <RenderPassType TRenderPass>
    class PassDeclarationBuilder {
        using ViewFactoryMethod = std::function<void(
            ID3D12Device*,
            const DescriptorHandle &,
            const std::unordered_map<std::string, std::unique_ptr<RenderResource>> &)>;

    public:
#pragma region Single View R/W
        template <RenderResourceUsage Usage> requires RenderResourceReadUsage<Usage>
        PassDeclarationBuilder& Read(
            const std::string &                  resourceName,
            DescriptorView<Usage> TRenderPass::* field,
            const DescriptorViewDesc &           desc                = std::monostate {},
            const std::optional<std::string>     counterResourceName = std::nullopt) {
            auto binding = std::make_shared<PassBinding<DescriptorView<Usage>>>(field);
            DeclareView<Usage>(resourceName, desc, counterResourceName, binding);
            passDeclaration.Resources.emplace_back(
                PassResourceDeclaration {
                    .method = PassResourceDeclaration::PassResourceUsingMethod::View,
                    .resourceName = resourceName,
                    .resourceUsage = Usage,
                    .resourceBinding = std::move(binding),
                }
            );

            return *this;
        }

        template <RenderResourceUsage Usage> requires RenderResourceWriteUsage<Usage>
        PassDeclarationBuilder& Write(
            const std::string &                  resourceName,
            DescriptorView<Usage> TRenderPass::* field,
            const DescriptorViewDesc &           desc                = std::monostate {},
            const std::optional<std::string>     counterResourceName = std::nullopt) {
            auto binding = std::make_shared<PassBinding<DescriptorView<Usage>>>(field);
            DeclareView<Usage>(resourceName, desc, counterResourceName, binding);
            passDeclaration.Resources.emplace_back(
                PassResourceDeclaration {
                    .method = PassResourceDeclaration::PassResourceUsingMethod::View,
                    .resourceName = resourceName,
                    .resourceUsage = Usage,
                    .resourceBinding = std::move(binding),
                }
            );

            return *this;
        }
#pragma endregion

#pragma region Single GPUAddress R/W
        PassDeclarationBuilder& Read(
            const std::string &                      resourceName,
            D3D12_GPU_VIRTUAL_ADDRESS TRenderPass::* field,
            const RenderResourceUsage                resourceUsage) {
            passDeclaration.Resources.emplace_back(
                PassResourceDeclaration {
                    .method = PassResourceDeclaration::PassResourceUsingMethod::GPUAddress,
                    .resourceName = resourceName,
                    .resourceUsage = resourceUsage,
                    .resourceBinding = std::make_shared<PassBinding<D3D12_GPU_VIRTUAL_ADDRESS>>(field)
                }
            );

            return *this;
        }

        PassDeclarationBuilder& Write(
            const std::string &                      resourceName,
            D3D12_GPU_VIRTUAL_ADDRESS TRenderPass::* field,
            const RenderResourceUsage                resourceUsage) {
            passDeclaration.Resources.emplace_back(
                PassResourceDeclaration {
                    .method = PassResourceDeclaration::PassResourceUsingMethod::GPUAddress,
                    .resourceName = resourceName,
                    .resourceUsage = resourceUsage,
                    .resourceBinding = std::make_shared<PassBinding<D3D12_GPU_VIRTUAL_ADDRESS>>(field),
                }
            );

            return *this;
        }
#pragma endregion

#pragma region Single Resource* R/W
        PassDeclarationBuilder& Read(
            const std::string &            resourceName,
            RenderResource* TRenderPass::* field,
            const RenderResourceUsage      resourceUsage) {
            passDeclaration.Resources.emplace_back(
                PassResourceDeclaration {
                    .method = PassResourceDeclaration::PassResourceUsingMethod::Resource,
                    .resourceName = resourceName,
                    .resourceUsage = resourceUsage,
                    .resourceBinding = std::make_shared<PassBinding<RenderResource*>>(field)
                }
            );

            return *this;
        }

        PassDeclarationBuilder& Write(
            const std::string &            resourceName,
            RenderResource* TRenderPass::* field,
            const RenderResourceUsage      resourceUsage) {
            passDeclaration.Resources.emplace_back(
                PassResourceDeclaration {
                    .method = PassResourceDeclaration::PassResourceUsingMethod::Resource,
                    .resourceName = resourceName,
                    .resourceUsage = resourceUsage,
                    .resourceBinding = std::make_shared<PassBinding<RenderResource*>>(field),
                }
            );

            return *this;
        }
#pragma endregion

#pragma region Array View R/W
        template <size_t I = 0, size_t N, RenderResourceUsage Usage> requires RenderResourceReadUsage<Usage>
        PassDeclarationBuilder& ReadArray(
            const std::string &                  resourceName,
            DescriptorView<Usage> (TRenderPass::*field)[N],
            const DescriptorViewDesc &           desc                = std::monostate {},
            const std::optional<std::string>     counterResourceName = std::nullopt) {
            for (size_t i = 0; i < N; ++i) {
                const std::string indexedName = resourceName + "[" + std::to_string(i + I) + "]";
                auto              binding     = std::make_shared<PassArrayBinding<DescriptorView<Usage>, N>>(field, i);
                DeclareView<Usage>(indexedName, desc, counterResourceName, binding);
                passDeclaration.Resources.emplace_back(
                    PassResourceDeclaration {
                        .method = PassResourceDeclaration::PassResourceUsingMethod::View,
                        .resourceName = indexedName,
                        .resourceUsage = Usage,
                        .resourceBinding = std::move(binding),
                    }
                );
            }

            return *this;
        }

        template <size_t I = 0, size_t N, RenderResourceUsage Usage> requires RenderResourceWriteUsage<Usage>
        PassDeclarationBuilder& WriteArray(
            const std::string &                  resourceName,
            DescriptorView<Usage> (TRenderPass::*field)[N],
            const DescriptorViewDesc &           desc                = std::monostate {},
            const std::optional<std::string>     counterResourceName = std::nullopt) {
            for (size_t i = 0; i < N; ++i) {
                const std::string indexedName = resourceName + "[" + std::to_string(i + I) + "]";
                auto              binding     = std::make_shared<PassArrayBinding<DescriptorView<Usage>, N>>(field, i);
                DeclareView<Usage>(indexedName, desc, counterResourceName, binding);
                passDeclaration.Resources.emplace_back(
                    PassResourceDeclaration {
                        .method = PassResourceDeclaration::PassResourceUsingMethod::View,
                        .resourceName = indexedName,
                        .resourceUsage = Usage,
                        .resourceBinding = std::move(binding),
                    }
                );
            }

            return *this;
        }
#pragma endregion

#pragma region Array GPUAddress R/W
#pragma region Array View R/W
        template <size_t I = 0, size_t N>
        PassDeclarationBuilder& ReadArray(
            const std::string &                      resourceName,
            D3D12_GPU_VIRTUAL_ADDRESS (TRenderPass::*field)[N],
            const RenderResourceUsage                resourceUsage) {
            for (size_t i = 0; i < N; ++i) {
                const std::string indexedName = resourceName + "[" + std::to_string(i + I) + "]";
                auto              binding     = std::make_shared<PassArrayBinding<D3D12_GPU_VIRTUAL_ADDRESS, N>>(field, i);
                passDeclaration.Resources.emplace_back(
                    PassResourceDeclaration {
                        .method = PassResourceDeclaration::PassResourceUsingMethod::GPUAddress,
                        .resourceName = indexedName,
                        .resourceUsage = resourceUsage,
                        .resourceBinding = std::move(binding),
                    }
                );
            }

            return *this;
        }

        template <size_t I = 0, size_t N>
        PassDeclarationBuilder& WriteArray(
            const std::string &                      resourceName,
            D3D12_GPU_VIRTUAL_ADDRESS (TRenderPass::*field)[N],
            const RenderResourceUsage                resourceUsage) {
            for (size_t i = 0; i < N; ++i) {
                const std::string indexedName = resourceName + "[" + std::to_string(i + I) + "]";
                auto              binding     = std::make_shared<PassArrayBinding<D3D12_GPU_VIRTUAL_ADDRESS, N>>(field, i);
                passDeclaration.Resources.emplace_back(
                    PassResourceDeclaration {
                        .method = PassResourceDeclaration::PassResourceUsingMethod::GPUAddress,
                        .resourceName = indexedName,
                        .resourceUsage = resourceUsage,
                        .resourceBinding = std::move(binding),
                    }
                );
            }

            return *this;
        }
#pragma endregion
#pragma endregion

#pragma region Array Resource* R/W
#pragma region Array View R/W
        template <size_t I = 0, size_t N>
        PassDeclarationBuilder& ReadArray(
            const std::string &            resourceName,
            RenderResource* (TRenderPass::*field)[N],
            const RenderResourceUsage      resourceUsage) {
            for (size_t i = 0; i < N; ++i) {
                const std::string indexedName = resourceName + "[" + std::to_string(i + I) + "]";
                auto              binding     = std::make_shared<PassArrayBinding<RenderResource*, N>>(field, i);
                passDeclaration.Resources.emplace_back(
                    PassResourceDeclaration {
                        .method = PassResourceDeclaration::PassResourceUsingMethod::Resource,
                        .resourceName = indexedName,
                        .resourceUsage = resourceUsage,
                        .resourceBinding = std::move(binding),
                    }
                );
            }

            return *this;
        }

        template <size_t I = 0, size_t N>
        PassDeclarationBuilder& WriteArray(
            const std::string &            resourceName,
            RenderResource* (TRenderPass::*field)[N],
            const RenderResourceUsage      resourceUsage) {
            for (size_t i = 0; i < N; ++i) {
                const std::string indexedName = resourceName + "[" + std::to_string(i + I) + "]";
                auto              binding     = std::make_shared<PassArrayBinding<RenderResource*, N>>(field, i);
                passDeclaration.Resources.emplace_back(
                    PassResourceDeclaration {
                        .method = PassResourceDeclaration::PassResourceUsingMethod::Resource,
                        .resourceName = indexedName,
                        .resourceUsage = resourceUsage,
                        .resourceBinding = std::move(binding),
                    }
                );
            }

            return *this;
        }
#pragma endregion
#pragma endregion

        template <RenderPassType TDependency>
        PassDeclarationBuilder& DependsAfter() {
            passDeclaration.PassDependencies.emplace_back(typeid(TDependency));
            return *this;
        }

        PassDeclarationBuilder& SideEffect() {
            passDeclaration.hasSideEffect = true;
            return *this;
        }

        [[nodiscard]]
        PassDeclaration Build() {
            return std::move(passDeclaration);
        }

    private:
        friend class RenderPipelineBuilder;

        std::function<void(
                const std::string &,
                RenderResourceUsage,
                const std::optional<std::string> &,
                const DescriptorViewDesc &,
                const std::weak_ptr<IPassBinding> &,
                ViewFactoryMethod)
        > registerView;

        PassDeclaration passDeclaration {
            .passType = typeid(TRenderPass),
        };

        template <typename... CtorArgs>
        void Construct(CtorArgs &&... ctorArgs) {
            passDeclaration.factory = [args = std::make_tuple(std::forward<CtorArgs>(ctorArgs)...)]() mutable {
                return std::apply(
                    []<typename... TArg>(TArg &&... a) {
                        return std::make_unique<TRenderPass>(std::forward<TArg>(a)...);
                    },
                    std::move(args)
                );
            };
        }

        template <RenderResourceUsage Usage>
        void DeclareView(
            const std::string &                  resourceName,
            const DescriptorViewDesc &           desc,
            const std::optional<std::string>     counterResourceName,
            const std::shared_ptr<IPassBinding> &binding) {
            registerView(
                resourceName,
                Usage,
                counterResourceName,
                desc,
                binding,
                [=](
            ID3D12Device*                                                           device,
            const DescriptorHandle &                                                handle,
            const std::unordered_map<std::string, std::unique_ptr<RenderResource>> &resources) {
                    const auto resource = resources.at(resourceName).get()->GetResource();

                    if constexpr ((Usage & RenderResourceUsage::AllShaderResource) != RenderResourceUsage::None) {
                        device->CreateShaderResourceView(resource, desc.index() == 0 ? nullptr : reinterpret_cast<const D3D12_SHADER_RESOURCE_VIEW_DESC*>(&desc), handle.cpuHandle);
                    } else if constexpr (Usage == RenderResourceUsage::RenderTarget) {
                        device->CreateRenderTargetView(resource, desc.index() == 0 ? nullptr : reinterpret_cast<const D3D12_RENDER_TARGET_VIEW_DESC*>(&desc), handle.cpuHandle);
                    } else if constexpr (Usage == RenderResourceUsage::DepthWrite || Usage == RenderResourceUsage::DepthRead) {
                        device->CreateDepthStencilView(resource, desc.index() == 0 ? nullptr : reinterpret_cast<const D3D12_DEPTH_STENCIL_VIEW_DESC*>(&desc), handle.cpuHandle);
                    } else if constexpr (Usage == RenderResourceUsage::UnorderedAccess) {
                        const auto counterResource = counterResourceName.has_value() ? resources.at(counterResourceName.value()).get()->GetResource() : nullptr;
                        device->CreateUnorderedAccessView(resource, counterResource, desc.index() == 0 ? nullptr : reinterpret_cast<const D3D12_UNORDERED_ACCESS_VIEW_DESC*>(&desc), handle.cpuHandle);
                    } else if constexpr (Usage == RenderResourceUsage::ConstantBuffer) {
                        auto cbvDesc           = *reinterpret_cast<const D3D12_CONSTANT_BUFFER_VIEW_DESC*>(&desc);
                        cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
                        device->CreateConstantBufferView(&cbvDesc, handle.cpuHandle);
                    }
                }
            );
        }

        template <typename TMember>
        struct PassBinding : IPassBinding {
            explicit               PassBinding(TMember TRenderPass::* member) : member(member) {}
            TMember TRenderPass::* member;
            TRenderPass*           instance = nullptr;

            void Target(void* pass) override { instance = static_cast<TRenderPass*>(pass); }
            void InjectValue(const void* resource) override { instance->*member = *static_cast<const TMember*>(resource); }
        };

        template <typename TMember, size_t N>
        struct PassArrayBinding : IPassBinding {
            TMember (TRenderPass::*arrayField)[N];
            size_t                 index;
            TRenderPass*           instance = nullptr;

            PassArrayBinding(
                TMember (TRenderPass::*field)[N],
                const size_t           index)
            : arrayField(field),
              index(index) {}

            void Target(void* pass) override { instance = static_cast<TRenderPass*>(pass); }
            void InjectValue(const void* resource) override { (instance->*arrayField)[index] = *static_cast<const TMember*>(resource); }
        };
    };
}
