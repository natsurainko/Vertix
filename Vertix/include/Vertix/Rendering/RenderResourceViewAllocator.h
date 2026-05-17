//
// Created by Natsurainko on 2026/4/30.
//

#ifndef VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H
#define VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H

#include <memory>

#include "RenderResource.h"
#include "RenderResourceView.h"
#include "Vertix/Graphics/DescriptorHeap.h"

namespace Vertix {
    class RenderResourceViewAllocator {
    public:
        explicit RenderResourceViewAllocator(const GraphicsDevice* graphicsDevice)
        : graphicsDevice(graphicsDevice), d3d12Device(graphicsDevice->GetD3D12Device()) {}

        VERTIX_API void InitRenderTargetDescriptorHeap(UINT maxDescriptors = 32);
        VERTIX_API void InitDepthStencilDescriptorHeap(UINT maxDescriptors = 32);
        VERTIX_API void InitSharedDescriptorHeap(UINT maxDescriptors = 32);

        [[nodiscard]] DescriptorHeap* GetRenderTargetDescriptorHeap() const { return rtvDescriptorHeap.get(); }
        [[nodiscard]] DescriptorHeap* GetDepthStencilDescriptorHeap() const { return dsvDescriptorHeap.get(); }
        [[nodiscard]] DescriptorHeap* GetSharedDescriptorHeap() const { return sharedDescriptorHeap.get(); }

        [[nodiscard]]
        VERTIX_API DescriptorHeapHandle CreateView(
            const RenderResource* resource,
            const RenderResourceViewDesc &desc,
            const RenderResource* counterResource = nullptr) const;

    private:
        const GraphicsDevice* graphicsDevice;
        Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device;

        std::unique_ptr<DescriptorHeap> rtvDescriptorHeap = nullptr;
        std::unique_ptr<DescriptorHeap> dsvDescriptorHeap = nullptr;
        std::unique_ptr<DescriptorHeap> sharedDescriptorHeap = nullptr;
    };
}

#endif //VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H
