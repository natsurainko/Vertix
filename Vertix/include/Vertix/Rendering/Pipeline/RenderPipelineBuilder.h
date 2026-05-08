//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPIPELINEBUILDER_H
#define VERTIX_RENDERPIPELINEBUILDER_H

#include <functional>
#include <memory>
#include <unordered_set>

#include "PassRequirementBuilder.h"
#include "RenderPipeline.h"
#include "Vertix/Rendering/RenderResourceViewDesc.h"

namespace Vertix {
    template<typename TContext>
    class RenderPipelineBuilder {
    public:
        class PipelineTextureDeclarationCollection {
        public:
            template<RenderResourceAccessor Accessor>
            void Add(
                const std::string &id,
                const D3D12_RESOURCE_DESC &desc,
                const bool resizedWithSwapChain = true,
                const D3D12_CLEAR_VALUE* clearValue = nullptr)
            {
                entries.emplace(id, PipelineTextureDeclaration {
                    .accessor = Accessor,
                    .clearValue = clearValue ? std::optional(*clearValue) : std::nullopt,
                    .resourceDesc = desc,
                    .resizedWithSwapChain = resizedWithSwapChain,
                    .factory = [
                        capturedDesc = desc,
                        capturedClear = clearValue ? std::optional(*clearValue) : std::nullopt
                    ](const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device) -> std::unique_ptr<RenderTextureBase> {
                        return capturedClear.has_value()
                            ? RenderTextureBase::Create<Accessor>(d3d12Device, capturedDesc, capturedClear)
                            : RenderTextureBase::Create<Accessor>(d3d12Device, capturedDesc);
                    }
                });
            }
        private:
            friend class RenderPipelineBuilder;
            struct PipelineTextureDeclaration {
                RenderResourceAccessor accessor{};
                std::optional<D3D12_CLEAR_VALUE> clearValue;
                D3D12_RESOURCE_DESC resourceDesc{};
                bool resizedWithSwapChain{};

                std::function<std::unique_ptr<RenderTextureBase>(const Microsoft::WRL::ComPtr<ID3D12Device>&)> factory;
            };

            std::unordered_map<std::string, PipelineTextureDeclaration> entries;
        };
        class PipelineViewDeclarationCollection {
        public:
            template<RenderResourceAccessor Accessor> requires SingleAccessor<Accessor>
            void Add(
                const std::string& viewId,
                const std::string& textureId,
                const ViewDescType<Accessor>* desc = nullptr,
                ID3D12Resource* counterResource = nullptr)
            {
                if constexpr (Accessor == RenderTarget) {
                    rtvDecls.emplace(viewId, RtvDecl { textureId, desc ? std::optional{*desc} : std::nullopt });
                } else if constexpr (Accessor == DepthStencil) {
                    dsvDecls.emplace(viewId, DsvDecl { textureId, desc ? std::optional{*desc} : std::nullopt });
                } else if constexpr (Accessor == UnorderedAccess) {
                    uavDecls.emplace(viewId, UavDecl { textureId, desc ? std::optional{*desc} : std::nullopt, counterResource });
                } else if constexpr (Accessor == ShaderResource) {
                    srvDecls.emplace(viewId, SrvDecl { textureId, desc ? std::optional{*desc} : std::nullopt });
                } else { assert(false && "Not supported accessor"); }
            }
        private:
            friend class RenderPipelineBuilder;
            struct RtvDecl { std::string textureId; std::optional<D3D12_RENDER_TARGET_VIEW_DESC>    desc; };
            struct DsvDecl { std::string textureId; std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC>    desc; };
            struct UavDecl { std::string textureId; std::optional<D3D12_UNORDERED_ACCESS_VIEW_DESC> desc; ID3D12Resource* counterResource = nullptr; };
            struct SrvDecl { std::string textureId; std::optional<D3D12_SHADER_RESOURCE_VIEW_DESC>  desc; };

            std::unordered_map<std::string, RtvDecl> rtvDecls;
            std::unordered_map<std::string, DsvDecl> dsvDecls;
            std::unordered_map<std::string, UavDecl> uavDecls;
            std::unordered_map<std::string, SrvDecl> srvDecls;
        };
        class PipelinePassDeclarationCollection {
        public:
            template<RenderPassType<TContext> TRenderPass, typename... CtorArgs>
            void Add(
                const std::function<void(PassRequirementBuilder&)> &declareFunc,
                CtorArgs&&... ctorArgs)
            {
                PassRequirementBuilder requirementBuilder;
                declareFunc(requirementBuilder);

                entries.push_back(PipelinePassDeclaration {
                    .requirement = requirementBuilder.Build(),
                    .factory     = [args = std::make_tuple(std::forward<CtorArgs>(ctorArgs)...)]() mutable {
                        return std::apply([](auto&&... a) {
                            return std::make_unique<TRenderPass>(std::forward<decltype(a)>(a)...);
                        }, std::move(args));
                    }
                });
            }
        private:
            friend class RenderPipelineBuilder;
            struct PipelinePassDeclaration {
                PassRequirementBuilder::PassRequirement requirement;
                std::function<std::unique_ptr<RenderPass<TContext>>()> factory;
            };

            std::vector<PipelinePassDeclaration> entries;
        };
        struct PipelineSwapChainDeclaration {
            SwapChain* swapChainPtr = nullptr;
            std::optional<D3D12_RENDER_TARGET_VIEW_DESC> frameRTVDesc;
        };
        struct PipelineDescriptorDeclaration {
            UINT reservedRTVDescriptorCount = 0;
            UINT reservedDSVDescriptorCount = 0;
            UINT reservedSharedDescriptorCount = 0;
        };

        explicit RenderPipelineBuilder(
            GraphicsDevice* graphicsDevice,
            FrameCommandList* frameCommandList,
            TContext* context)
        : graphicsDevice(graphicsDevice), frameCommandList(frameCommandList), context(context) {}

        PipelineTextureDeclarationCollection Textures;
        PipelineViewDeclarationCollection Views;
        PipelinePassDeclarationCollection Passes;
        PipelineSwapChainDeclaration SwapChain;
        PipelineDescriptorDeclaration Descriptor;

        [[nodiscard]]
        std::unique_ptr<RenderPipeline<TContext>> Build() {
            if (!SwapChain.swapChainPtr) throw std::runtime_error("No swapChain available");

            const auto d3d12Device = graphicsDevice->GetD3D12Device();
            const auto d3d12CommandList = frameCommandList->GetD3D12GraphicsCommandList();
            const auto swapChain = SwapChain.swapChainPtr;
            const auto frameRTVDesc = SwapChain.frameRTVDesc.has_value() ? &SwapChain.frameRTVDesc.value() : nullptr;

            std::unique_ptr<RenderPipeline<TContext>> renderPipeline(new RenderPipeline<TContext>(graphicsDevice, frameCommandList, swapChain, context));

            // Build the declared texture
            std::unordered_set<std::string> &resizableTextureIds = renderPipeline->resizableTextureIds;
            std::unordered_map<std::string, D3D12_RESOURCE_STATES> baselineTextureStates;
            for (const auto &[textureId, textureDecl] : Textures.entries) {
                auto texture = textureDecl.factory(d3d12Device);
                baselineTextureStates.emplace(textureId, texture->GetCurrentState());
                renderPipeline->textures.emplace(textureId, std::move(texture));
                if (textureDecl.resizedWithSwapChain) {
                    resizableTextureIds.insert(textureId);
                }
            }

            // Build the declared texture view
            std::unordered_map<std::string, std::string> &viewIdToTextureId = renderPipeline->viewIdToTextureId;
            renderPipeline->viewAllocator = std::make_unique<RenderResourceViewAllocator>(graphicsDevice);
            {
                auto viewAccessorValidator = [&](
                    const std::string& viewId,
                    const std::string& textureId,
                    const RenderResourceAccessor requiredAccessor)
                {
                    const auto it = Textures.entries.find(textureId);
                    if (it == Textures.entries.end())
                        throw std::runtime_error("View '" + viewId + "' references undeclared texture '" + textureId + "'");

                    if ((it->second.accessor & requiredAccessor) == 0)
                        throw std::runtime_error(
                            "View '" + viewId + "' requires accessor 0x" +
                            std::to_string(requiredAccessor) +
                            " but texture '" + textureId +
                            "' was declared with accessor 0x" +
                            std::to_string(it->second.accessor));
                };

                auto &viewAllocator = renderPipeline->viewAllocator;
                viewAllocator->InitRenderTargetDescriptorHeap(
                    static_cast<UINT>(Views.rtvDecls.size()) + Descriptor.reservedRTVDescriptorCount + swapChain->GetFrameCount() );
                viewAllocator->InitDepthStencilDescriptorHeap(
                    static_cast<UINT>(Views.dsvDecls.size()) + Descriptor.reservedDSVDescriptorCount);
                viewAllocator->InitSharedDescriptorHeap(
                    static_cast<UINT>(Views.srvDecls.size()) + static_cast<UINT>(Views.uavDecls.size()) + Descriptor.reservedSharedDescriptorCount);

                for (const auto &[viewId, viewDecl] : Views.rtvDecls) {
                    viewAccessorValidator(viewId, viewDecl.textureId, RenderTarget);
                    const auto texture = renderPipeline->textures.at(viewDecl.textureId).get();
                    const auto desc    = viewDecl.desc.has_value() ? &viewDecl.desc.value() : nullptr;
                    renderPipeline->rtvViews.emplace(viewId, viewAllocator->template CreateViewForTexture<RenderTarget>(texture, desc));
                    viewIdToTextureId.emplace(viewId, viewDecl.textureId);
                }
                for (const auto &[viewId, viewDecl] : Views.dsvDecls) {
                    viewAccessorValidator(viewId, viewDecl.textureId, DepthStencil);
                    const auto texture = renderPipeline->textures.at(viewDecl.textureId).get();
                    const auto desc    = viewDecl.desc.has_value() ? &viewDecl.desc.value() : nullptr;
                    renderPipeline->dsvViews.emplace(viewId, viewAllocator->template CreateViewForTexture<DepthStencil>(texture, desc));
                    viewIdToTextureId.emplace(viewId, viewDecl.textureId);
                }
                for (const auto &[viewId, viewDecl] : Views.uavDecls) {
                    viewAccessorValidator(viewId, viewDecl.textureId, UnorderedAccess);
                    const auto texture = renderPipeline->textures.at(viewDecl.textureId).get();
                    const auto desc    = viewDecl.desc.has_value() ? &viewDecl.desc.value() : nullptr;
                    renderPipeline->uavViews.emplace(viewId, viewAllocator->CreateUAVViewForTexture(texture, desc, viewDecl.counterResource));
                    viewIdToTextureId.emplace(viewId, viewDecl.textureId);
                }
                for (const auto &[viewId, viewDecl] : Views.srvDecls) {
                    viewAccessorValidator(viewId, viewDecl.textureId, ShaderResource);
                    const auto texture = renderPipeline->textures.at(viewDecl.textureId).get();
                    const auto desc    = viewDecl.desc.has_value() ? &viewDecl.desc.value() : nullptr;
                    renderPipeline->srvViews.emplace(viewId, viewAllocator->template CreateViewForTexture<ShaderResource>(texture, desc));
                    viewIdToTextureId.emplace(viewId, viewDecl.textureId);
                }

                for (UINT i = 0; i < swapChain->GetFrameCount(); ++i) {
                    renderPipeline->frameRTVs.emplace_back(viewAllocator->CreateViewForSwapChainBuffer(swapChain->GetBuffer(i), frameRTVDesc));
                }
                renderPipeline->currentFrameRTV = &renderPipeline->frameRTVs[swapChain->GetCurrentFrameIndex()];
            }

            // Build the declared render pass
            for (const auto &[requirement, factory] : Passes.entries) {
                PassInitializationContext initializationContext;
                initializationContext.currentFrameRTV = &renderPipeline->currentFrameRTV;
                initializationContext.viewAllocator = renderPipeline->viewAllocator.get();

                for (const auto &view : requirement.viewRequirements) {
                    switch (view.viewAccessor) {
                        case RenderTarget:
                            if (!renderPipeline->rtvViews.contains(view.viewId))
                                throw std::runtime_error(
                                    "Pass requires RTV '" + view.viewId + "' which was not declared in Views");
                            initializationContext.rtvViews.emplace(view.viewId, &renderPipeline->rtvViews.at(view.viewId));
                            break;
                        case DepthStencil:
                            if (!renderPipeline->dsvViews.contains(view.viewId))
                                throw std::runtime_error(
                                    "Pass requires DSV '" + view.viewId + "' which was not declared in Views");
                            initializationContext.dsvViews.emplace(view.viewId, &renderPipeline->dsvViews.at(view.viewId));
                            break;
                        case UnorderedAccess:
                            if (!renderPipeline->uavViews.contains(view.viewId))
                                throw std::runtime_error(
                                    "Pass requires UAV '" + view.viewId + "' which was not declared in Views");
                            initializationContext.uavViews.emplace(view.viewId, &renderPipeline->uavViews.at(view.viewId));
                            break;
                        case ShaderResource:
                            if (!renderPipeline->srvViews.contains(view.viewId))
                                throw std::runtime_error(
                                    "Pass requires SRV '" + view.viewId + "' which was not declared in Views");
                            initializationContext.srvViews.emplace(view.viewId, &renderPipeline->srvViews.at(view.viewId));
                            break;
                        default:
                            assert(false && "Not supported accessor");
                            break;
                    }
                }

                renderPipeline->passes.emplace_back(factory());
                auto* renderPass = renderPipeline->passes.back().get();
                renderPass->Initialize(graphicsDevice, initializationContext, context);
            }

            // Compile the barriers
            auto simulatedTextureStates = baselineTextureStates;
            renderPipeline->prePassBarriers.resize(Passes.entries.size());
            renderPipeline->prePassBarrierRecipes.resize(Passes.entries.size());
            {
                for (size_t i = 0; i < Passes.entries.size(); ++i) {
                    auto& barriers = renderPipeline->prePassBarriers[i];
                    auto& barrierRecipes = renderPipeline->prePassBarrierRecipes[i];

                    std::unordered_set<std::string> seenTextures;
                    for (auto& req : Passes.entries[i].requirement.viewRequirements) {
                        const auto& texId = viewIdToTextureId.at(req.viewId);
                        assert(seenTextures.insert(texId).second && "Same texture required in two different states within one pass");
                        auto& currentState = simulatedTextureStates.at(texId);

                        if (currentState != req.requiredState) {
                            auto* resource = renderPipeline->textures.at(texId)->GetResource();
                            barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resource, currentState, req.requiredState));
                            barrierRecipes.emplace_back(texId, currentState, req.requiredState);
                            currentState = req.requiredState;
                        }
                    }
                }

                for (auto& [texId, finalState] : simulatedTextureStates) {
                    if (finalState != baselineTextureStates.at(texId)) {
                        auto* resource = renderPipeline->textures.at(texId)->GetResource();
                        const auto restoreState = baselineTextureStates.at(texId);
                        renderPipeline->postPassBarriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resource, finalState, restoreState));
                        renderPipeline->postPassBarrierRecipes.emplace_back(texId, finalState, restoreState);
                    }
                }
            }

            return std::move(renderPipeline);
        }

    private:
        GraphicsDevice*   graphicsDevice   = nullptr;
        FrameCommandList* frameCommandList = nullptr;
        TContext*         context          = nullptr;
    };
}

#endif //VERTIX_RENDERPIPELINEBUILDER_H
