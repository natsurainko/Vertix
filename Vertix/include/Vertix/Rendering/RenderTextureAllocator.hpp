//
// Created by Natsurainko on 2026/4/30.
//

#ifndef VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H
#define VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H

#include "Vertix/Graphics/DescriptorHeap.h"

namespace Vertix {
    class RenderTextureAllocator {
    public:
        explicit RenderTextureAllocator(const GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {}

        void InitRenderTargetDescriptorHeap(const UINT maxDescriptors = 32) {
            rtvDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxDescriptors, false);
        }

        void InitDepthStencilDescriptorHeap(const UINT maxDescriptors = 32) {
            dsvDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, maxDescriptors, false);
        }

        void InitSharedDescriptorHeap(const UINT maxDescriptors = 32) {
            sharedDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxDescriptors, true);
        }

        [[nodiscard]]
        DescriptorHeap* GetRenderTargetDescriptorHeap() const {
            return rtvDescriptorHeap.get();
        }

        [[nodiscard]]
        DescriptorHeap* GetDepthStencilDescriptorHeap() const {
            return dsvDescriptorHeap.get();
        }

        [[nodiscard]]
        DescriptorHeap* GetUnorderedAccessDescriptorHeap() const {
            return sharedDescriptorHeap.get();
        }

        [[nodiscard]]
        DescriptorHeap* GetShaderResourceDescriptorHeap() const {
            return sharedDescriptorHeap.get();
        }

        [[nodiscard]]
        const GraphicsDevice* GetGraphicsDevice() const {
            return graphicsDevice;
        }

    private:
        const GraphicsDevice* graphicsDevice;

        std::unique_ptr<DescriptorHeap> rtvDescriptorHeap = nullptr;
        std::unique_ptr<DescriptorHeap> dsvDescriptorHeap = nullptr;
        std::unique_ptr<DescriptorHeap> sharedDescriptorHeap = nullptr;
    };
}

#endif //VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H
