//
// Created by Natsurainko on 2026/1/27.
//

#ifndef VERTIX_RENDERPASS_H
#define VERTIX_RENDERPASS_H

namespace Vertix {
    class GraphicsDevice;

    template <typename TContext>
    class RenderPass {
    public:
        virtual ~RenderPass() = default;

        virtual void Initialize(GraphicsDevice* device, TContext* context) {
            renderContext = context;
            graphicsDevice = device;
        }

        virtual void Execute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList) = 0;
        virtual void Resize(const Vector2D<UINT> &size) {}
    protected:
        TContext* renderContext = nullptr;
        GraphicsDevice* graphicsDevice = nullptr;

        bool enabled = true;
    };
}

#endif //VERTIX_RENDERPASS_H