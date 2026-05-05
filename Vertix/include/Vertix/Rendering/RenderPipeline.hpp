//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_RENDERPIPELINE_H
#define VERTIX_RENDERPIPELINE_H

#include <concepts>
#include <memory>
#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/Math/Vector2D.hpp"
#include "Vertix/Rendering/RenderPass.hpp"

namespace Vertix {
    template <typename TRenderPass, typename TContext>
    concept RenderPassType = std::derived_from<TRenderPass, RenderPass<TContext>>;

    template <typename TContext, RenderPassType<TContext> TRenderPass>
    using RenderPassPtr = std::unique_ptr<TRenderPass>;

    template <typename TContext>
    class RenderPipeline {
    protected:
        TContext* renderContext = nullptr;
        GraphicsDevice* graphicsDevice = nullptr;
        ID3D12GraphicsCommandList5* commandList = nullptr;

        std::vector<std::unique_ptr<RenderPass<TContext>>> renderPasses;

    public:
        RenderPipeline(GraphicsDevice* graphicsDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &graphicsCommandList)
            : graphicsDevice(graphicsDevice), commandList(graphicsCommandList.Get()) {}

        virtual ~RenderPipeline() {
            renderPasses.clear();
            delete renderContext;
        }

        template <RenderPassType<TContext> TRenderPass>
        void AddPass(RenderPassPtr<TContext, TRenderPass> renderPass) {
            if (renderPass) {
                renderPass->Initialize(graphicsDevice, renderContext);
                renderPasses.push_back(std::move(renderPass));
            }
        }

        template <RenderPassType<TContext> TRenderPass, typename... Args>
        TRenderPass* CreateAddPass(Args&&... args) {
            auto renderPass = std::make_unique<TRenderPass>(std::forward<Args>(args)...);
            TRenderPass* rawPtr = renderPass.get();
            AddPass(std::move(renderPass));
            return rawPtr;
        }

        template <RenderPassType<TContext> TRenderPass>
        bool RemovePass(TRenderPass* renderPass) {
            if (!renderPass) return false;

            auto iteration = std::find_if(renderPasses.begin(), renderPasses.end(),
                [renderPass](const auto& pass) {
                    return pass.get() == renderPass;
                });

            if (iteration != renderPasses.end()) {
                renderPasses.erase(iteration);
                return true;
            }

            return false;
        }

        virtual void Execute() {
            for (const auto &renderPass : renderPasses) {
                renderPass->Execute(commandList);
            }
        }

        virtual void Resize(const Vector2D<UINT> &size) {}

        [[nodiscard]]
        std::vector<std::unique_ptr<RenderPass<TContext>>>& GetPasses() const noexcept {
            return renderPasses;
        }

        [[nodiscard]]
        TContext* GetRenderContext() noexcept {
            return renderContext;
        }
    };
}

#endif //VERTIX_RENDERPIPELINE_H
