//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPIPELINEBUILDER_H
#define VERTIX_RENDERPIPELINEBUILDER_H

#include <functional>
#include <map>
#include <memory>
#include <unordered_set>
#include <variant>

#include "PassDeclarationBuilder.h"
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
                const D3D12_RESOURCE_DESC &resDesc,
                const bool resizedWithSwapChain = true,
                const D3D12_CLEAR_VALUE* clearValue = nullptr)
            {
                const auto result = entries.emplace(id, PipelineTextureDeclaration {
                    .accessor = Accessor,
                    .clearValue = clearValue ? std::optional(*clearValue) : std::nullopt,
                    .resourceDesc = resDesc,
                    .resizedWithSwapChain = resizedWithSwapChain,
                    .factory = [](const D3D12_RESOURCE_DESC& desc,
                                  const std::optional<D3D12_CLEAR_VALUE>& clear,
                                  const Microsoft::WRL::ComPtr<ID3D12Device>& device) -> std::unique_ptr<RenderTextureBase>
                    {
                        return clear.has_value()
                            ? RenderTextureBase::Create<Accessor>(device, desc, clear)
                            : RenderTextureBase::Create<Accessor>(device, desc);
                    }
                });

                if (result.second) entriesOrder.emplace_back(id);
            }
        private:
            friend class RenderPipelineBuilder;
            struct PipelineTextureDeclaration {
                RenderResourceAccessor accessor{};
                std::optional<D3D12_CLEAR_VALUE> clearValue;
                D3D12_RESOURCE_DESC resourceDesc{};
                bool resizedWithSwapChain{};

                std::function<std::unique_ptr<RenderTextureBase>(
                    const D3D12_RESOURCE_DESC&,
                    const std::optional<D3D12_CLEAR_VALUE>&,
                    const Microsoft::WRL::ComPtr<ID3D12Device>&
                )> factory;
            };

            std::map<std::string, PipelineTextureDeclaration> entries;
            std::vector<std::string> entriesOrder;
        };
        class PipelineViewDeclarationCollection {
        public:
            template<RenderResourceAccessor Accessor> requires SingleAccessor<Accessor>
            void AddExplicit(
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

            template<RenderResourceAccessor Accessor> requires SingleAccessor<Accessor>
            void AddExplicit(
                const std::string& viewId,
                const std::string& textureId,
                const ViewDescType<Accessor> desc,
                ID3D12Resource* counterResource = nullptr)
            {
                if constexpr (Accessor == RenderTarget) {
                    rtvDecls.emplace(viewId, RtvDecl { textureId, std::optional{desc} });
                } else if constexpr (Accessor == DepthStencil) {
                    dsvDecls.emplace(viewId, DsvDecl { textureId, std::optional{desc} });
                } else if constexpr (Accessor == UnorderedAccess) {
                    uavDecls.emplace(viewId, UavDecl { textureId, std::optional{desc}, counterResource });
                } else if constexpr (Accessor == ShaderResource) {
                    srvDecls.emplace(viewId, SrvDecl { textureId, std::optional{desc} });
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
            template<RenderPassType<TContext> TRenderPass, typename TDeclareFunc, typename... CtorArgs>
                requires std::invocable<TDeclareFunc, PassDeclarationBuilder<TRenderPass>&>
            void Add(
                TDeclareFunc&& declareFunc,
                CtorArgs&&... ctorArgs)
            {
                PassDeclarationBuilder<TRenderPass> declarationBuilder;
                declareFunc(declarationBuilder);

                entries.push_back(PipelinePassDeclaration {
                    .declaration = declarationBuilder.Build(),
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
                PassDeclaration declaration;
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
                auto texture = textureDecl.factory(textureDecl.resourceDesc, textureDecl.clearValue, d3d12Device);
                baselineTextureStates.emplace(textureId, texture->GetCurrentState());
                renderPipeline->textures.emplace(textureId, std::move(texture));
                if (textureDecl.resizedWithSwapChain) {
                    resizableTextureIds.insert(textureId);
                }
            }

            std::unordered_map<std::string, std::string> &viewIdToTextureId = renderPipeline->viewIdToTextureId;
            std::unordered_map<std::string, RenderResourceAccessor> explicitViewIdToAccessor;

            // 1. Collect viewId of explict texture view
            // 2. Validate explict views
            {
                auto viewAccessorValidator = [&](
                    const std::string& textureId,
                    const RenderResourceAccessor requiredAccessor)
                {
                    const auto it = Textures.entries.find(textureId);
                    if (it == Textures.entries.end())
                        throw std::runtime_error("The TextureId specified in the explicit view does not exist.");
                    if ((it->second.accessor & requiredAccessor) == 0)
                        throw std::runtime_error("The specified texture does not conform to the accessor of the explicit view.");
                };

                auto collectExplicitViews = [&](auto& declMap, RenderResourceAccessor accessor) {
                    for (const auto& [viewId, viewDecl] : declMap) {
                        viewAccessorValidator(viewDecl.textureId, accessor);
                        if (!viewIdToTextureId.emplace(viewId, viewDecl.textureId).second)
                            throw std::runtime_error("The viewId specified existed.");
                        explicitViewIdToAccessor.emplace(viewId, accessor);
                    }
                };

                collectExplicitViews(Views.rtvDecls, RenderTarget);
                collectExplicitViews(Views.dsvDecls, DepthStencil);
                collectExplicitViews(Views.uavDecls, UnorderedAccess);
                collectExplicitViews(Views.srvDecls, ShaderResource);
            }

            // 1. Collect view of implicit texture view
            // 2. Validate PassDataflow declarations
            for (const auto &[passDecl, factory] : Passes.entries) {
                std::unordered_set<std::string> seenTextureIds;
                for (const auto &dataflow : passDecl.dataflows) {
                    if (auto* implicitDecl = std::get_if<PassDataflowImplicitDeclaration>(&dataflow.declaration)) {
                        if (!renderPipeline->textures.contains(implicitDecl->textureId))
                            throw std::runtime_error("The TextureId specified in the implicit declaration does not exist.");
                        if (auto result = seenTextureIds.insert(implicitDecl->textureId); !result.second)
                            throw std::runtime_error("Only one dataflow can be declared within the same pass for the same texture.");

                        std::string implicitViewId = PassImplicitViewId(implicitDecl->textureId, implicitDecl->accessor);
                        if (viewIdToTextureId.contains(implicitViewId)) continue;

                        switch (implicitDecl->accessor) {
                            case RenderTarget:
                                Views.rtvDecls.emplace(implicitViewId, typename PipelineViewDeclarationCollection::RtvDecl(implicitDecl->textureId, std::nullopt));
                                break;
                            case DepthStencil:
                                Views.dsvDecls.emplace(implicitViewId, typename PipelineViewDeclarationCollection::DsvDecl(implicitDecl->textureId, std::nullopt));
                                break;
                            case UnorderedAccess:
                                Views.uavDecls.emplace(implicitViewId, typename PipelineViewDeclarationCollection::UavDecl(implicitDecl->textureId, std::nullopt, nullptr));
                                break;
                            case ShaderResource:
                                Views.srvDecls.emplace(implicitViewId, typename PipelineViewDeclarationCollection::SrvDecl(implicitDecl->textureId, std::nullopt));
                                break;
                            default:
                                throw std::runtime_error("Not supported accessor");
                                break;
                        }

                        auto _ = viewIdToTextureId.emplace(implicitViewId, implicitDecl->textureId);
                    }
                    else if (auto* explicitDecl = std::get_if<PassDataflowExplicitDeclaration>(&dataflow.declaration)) {
                        if (!viewIdToTextureId.contains(explicitDecl->viewId))
                            throw std::runtime_error("The ViewId specified in the explicit declaration does not exist.");
                        if (auto result = seenTextureIds.insert(viewIdToTextureId.at(explicitDecl->viewId)); !result.second)
                            throw std::runtime_error("Only one dataflow can be declared within the same pass for the same texture.");
                    }
                }
            }

            // Build the declared texture views
            renderPipeline->viewAllocator = std::make_unique<RenderResourceViewAllocator>(graphicsDevice);
            {
                auto &viewAllocator = renderPipeline->viewAllocator;
                viewAllocator->InitRenderTargetDescriptorHeap(
                    static_cast<UINT>(Views.rtvDecls.size()) + Descriptor.reservedRTVDescriptorCount + swapChain->GetFrameCount() );
                viewAllocator->InitDepthStencilDescriptorHeap(
                    static_cast<UINT>(Views.dsvDecls.size()) + Descriptor.reservedDSVDescriptorCount);
                viewAllocator->InitSharedDescriptorHeap(
                    static_cast<UINT>(Views.srvDecls.size()) + static_cast<UINT>(Views.uavDecls.size()) + Descriptor.reservedSharedDescriptorCount);

                auto buildViews = [&]<RenderResourceAccessor A>(auto& declMap, auto& viewMap) {
                    for (const auto& [viewId, viewDecl] : declMap) {
                        const auto* texture = renderPipeline->textures.at(viewDecl.textureId).get();
                        const auto* desc    = viewDecl.desc.has_value() ? &viewDecl.desc.value() : nullptr;
                        viewMap.emplace(viewId, viewAllocator->template CreateViewForTexture<A>(texture, desc));
                    }
                };

                buildViews.template operator()<RenderTarget>(Views.rtvDecls, renderPipeline->rtvViews);
                buildViews.template operator()<DepthStencil>(Views.dsvDecls, renderPipeline->dsvViews);

                // Build a rank table from Textures.entries
                std::unordered_map<std::string, std::size_t> textureRank;
                {
                    textureRank.reserve(Textures.entriesOrder.size());
                    std::size_t rank = 0;
                    for (const auto& texId : Textures.entriesOrder)
                        textureRank.emplace(texId, rank++);
                }

                auto makeSortedPtrs = [&]<typename TDecl>(const std::unordered_map<std::string, TDecl>& declMap) {
                    using Entry = std::pair<const std::string, TDecl>;
                    std::vector<const Entry*> ptrs;
                    ptrs.reserve(declMap.size());
                    for (const auto& e : declMap)
                        ptrs.push_back(&e);
                    std::sort(ptrs.begin(), ptrs.end(), [&](const Entry* a, const Entry* b) {
                        const auto kMax = (std::numeric_limits<std::size_t>::max)();
                        const auto ra = textureRank.contains(a->second.textureId) ? textureRank.at(a->second.textureId) : kMax;
                        const auto rb = textureRank.contains(b->second.textureId) ? textureRank.at(b->second.textureId) : kMax;
                        if (ra != rb) return ra < rb;
                        return a->first < b->first;
                    });
                    return ptrs;
                };

                for (const auto* ptr : makeSortedPtrs(Views.srvDecls)) {
                    const auto& [viewId, viewDecl] = *ptr;
                    const auto* texture = renderPipeline->textures.at(viewDecl.textureId).get();
                    const auto* desc    = viewDecl.desc.has_value() ? &viewDecl.desc.value() : nullptr;
                    renderPipeline->srvViews.emplace(viewId,
                        viewAllocator->template CreateViewForTexture<ShaderResource>(texture, desc));
                }

                for (const auto* ptr : makeSortedPtrs(Views.uavDecls)) {
                    const auto& [viewId, viewDecl] = *ptr;
                    const auto* texture = renderPipeline->textures.at(viewDecl.textureId).get();
                    const auto* desc    = viewDecl.desc.has_value() ? &viewDecl.desc.value() : nullptr;
                    renderPipeline->uavViews.emplace(viewId,
                        viewAllocator->CreateUAVViewForTexture(texture, desc, viewDecl.counterResource));
                }

                for (UINT i = 0; i < swapChain->GetFrameCount(); ++i) {
                    renderPipeline->frameRTVs.emplace_back(viewAllocator->CreateViewForSwapChainBuffer(swapChain->GetBuffer(i), frameRTVDesc));
                }
                renderPipeline->currentFrameRTV = &renderPipeline->frameRTVs[swapChain->GetCurrentFrameIndex()];
            }

            // Build the declared render pass
            for (const auto &[passDecl, factory] : Passes.entries) {
                renderPipeline->passes.emplace_back(factory());
                auto* renderPass = renderPipeline->passes.back().get();

                for (auto& dataflow : passDecl.dataflows) {
                    if (auto* swapChainDecl = std::get_if<PassDataflowSwapChainDeclaration>(&dataflow.declaration)) {
                        dataflow.dataflowBinding->Inject(renderPass, &renderPipeline->currentFrameRTV);
                        continue;
                    }

                    std::string viewId;
                    RenderResourceAccessor accessor;

                    if (auto* implicitDecl = std::get_if<PassDataflowImplicitDeclaration>(&dataflow.declaration)) {
                        accessor = implicitDecl->accessor;
                        viewId = PassImplicitViewId(implicitDecl->textureId, implicitDecl->accessor);
                    } else if (auto* explicitDecl = std::get_if<PassDataflowExplicitDeclaration>(&dataflow.declaration)) {
                        accessor = explicitViewIdToAccessor.at(explicitDecl->viewId);
                        viewId = explicitDecl->viewId;
                    }

                    switch (accessor) {
                        case RenderTarget:
                            dataflow.dataflowBinding->Inject(renderPass, &renderPipeline->rtvViews.at(viewId));
                            break;
                        case DepthStencil:
                            dataflow.dataflowBinding->Inject(renderPass, &renderPipeline->dsvViews.at(viewId));
                            break;
                        case UnorderedAccess:
                            dataflow.dataflowBinding->Inject(renderPass, &renderPipeline->uavViews.at(viewId));
                            break;
                        case ShaderResource:
                            dataflow.dataflowBinding->Inject(renderPass, &renderPipeline->srvViews.at(viewId));
                            break;
                        default:
                            assert(false && "Not supported accessor");
                            break;
                    }
                }

                renderPass->renderContext = context;
                renderPass->Initialize(d3d12Device.Get());
            }

            // Compile the barriers
            auto simulatedTextureStates = baselineTextureStates;
            renderPipeline->prePassBarriers.resize(Passes.entries.size());
            renderPipeline->prePassBarrierRecipes.resize(Passes.entries.size());
            {
                for (size_t i = 0; i < Passes.entries.size(); ++i) {
                    auto& barriers = renderPipeline->prePassBarriers[i];
                    auto& barrierRecipes = renderPipeline->prePassBarrierRecipes[i];

                    for (auto& dataflow : Passes.entries[i].declaration.dataflows) {
                        std::string textureId;
                        D3D12_RESOURCE_STATES targetState;

                        if (auto* implicitDecl = std::get_if<PassDataflowImplicitDeclaration>(&dataflow.declaration)) {
                            textureId = implicitDecl->textureId;
                            targetState = implicitDecl->DerivativeState(dataflow.direction);
                        } else if (auto* explicitDecl = std::get_if<PassDataflowExplicitDeclaration>(&dataflow.declaration)) {
                            textureId = viewIdToTextureId.at(explicitDecl->viewId);
                            targetState = explicitDecl->explicitState;
                        } else continue;

                        if (auto& currentState = simulatedTextureStates.at(textureId); currentState != targetState) {
                            auto* resource = renderPipeline->textures.at(textureId)->GetResource();
                            barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(resource, currentState, targetState));
                            barrierRecipes.emplace_back(textureId, currentState, targetState);
                            currentState = targetState;
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
