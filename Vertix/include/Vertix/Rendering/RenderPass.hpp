//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_RENDERPASS_H
#define VERTIX_RENDERPASS_H

#include <d3d12/d3d12.h>
#include <Vertix/Math/Vector2D.hpp>

namespace Vertix {
    class GraphicsDevice;

    template <typename TContext>
    class RenderPass {
    public:
        virtual ~RenderPass() = default;

        virtual void Initialize(
            GraphicsDevice* device,
            TContext* context)
        {
            renderContext = context;
            graphicsDevice = device;
        }

        virtual void Execute(ID3D12GraphicsCommandList5* commandList) = 0;
        virtual void Resize(const Vector2D<UINT> &size) {}
    protected:
        TContext* renderContext = nullptr;
        GraphicsDevice* graphicsDevice = nullptr;

        bool enabled = true;
    };
}

#endif //VERTIX_RENDERPASS_H
