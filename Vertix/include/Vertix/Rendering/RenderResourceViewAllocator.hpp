//
// Created by Natsurainko on 2026/4/30.
//

#ifndef VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H
#define VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H

#include <memory>

#include "RenderResourceAccessor.h"
#include "RenderResourceView.h"
#include "RenderResourceViewDesc.h"
#include "RenderTexture.hpp"
#include "Vertix/Graphics/DescriptorHeap.h"
#include "Vertix/Graphics/SwapChain.h"

namespace Vertix {
    class RenderResourceViewAllocator {
    public:
        explicit RenderResourceViewAllocator(const GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice), d3d12Device(graphicsDevice->GetD3D12Device()) {}

        void InitRenderTargetDescriptorHeap(const UINT maxDescriptors = 32) {
            rtvDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxDescriptors, false);
            rtvDescriptorHeap->GetDescriptorHeap()->SetName(L"ViewAllocator.RenderTarget.DescriptorHeap");
        }

        void InitDepthStencilDescriptorHeap(const UINT maxDescriptors = 32) {
            dsvDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, maxDescriptors, false);
            dsvDescriptorHeap->GetDescriptorHeap()->SetName(L"ViewAllocator.DepthStencil.DescriptorHeap");
        }

        void InitSharedDescriptorHeap(const UINT maxDescriptors = 32) {
            sharedDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxDescriptors, true);
            sharedDescriptorHeap->GetDescriptorHeap()->SetName(L"ViewAllocator.Shared.DescriptorHeap");
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

        template<RenderResourceAccessor A> requires SingleAccessor<A>
        RenderResourceView<A> CreateView(
            ID3D12Resource* resource,
            const ViewDescType<A>* desc = nullptr) const
        {
            RenderResourceView<A> view;
            if constexpr (A == RenderTarget) {
                view.heap = GetRenderTargetDescriptorHeap();
                view.heap->AllocDescriptorHandle(view.handle);
                view.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
                d3d12Device->CreateRenderTargetView(resource, desc, view.handle);
            }
            else if constexpr (A == DepthStencil) {
                view.heap = GetDepthStencilDescriptorHeap();
                view.heap->AllocDescriptorHandle(view.handle);
                view.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
                d3d12Device->CreateDepthStencilView(resource, desc, view.handle);
            }
            else if constexpr (A == UnorderedAccess) {
                view.heap = GetUnorderedAccessDescriptorHeap();
                view.heap->AllocDescriptorHandle(view.cpuHandle, view.gpuHandle);
                view.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
                view.cachedCounterResource = nullptr;
                d3d12Device->CreateUnorderedAccessView(resource, nullptr, desc, view.cpuHandle);
            }
            else if constexpr (A == ShaderResource) {
                view.heap = GetShaderResourceDescriptorHeap();
                view.heap->AllocDescriptorHandle(view.cpuHandle, view.gpuHandle);
                view.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
                d3d12Device->CreateShaderResourceView(resource, desc, view.cpuHandle);
            }

            return view;
        }

        template<RenderResourceAccessor A> requires SingleAccessor<A>
        RenderResourceView<A> CreateViewForTexture(
            const RenderTextureBase* texture,
            const ViewDescType<A>* desc = nullptr) const
        {
            assert(texture->GetAccessor() & A);
            return CreateView<A>(texture->GetResource(), desc);
        }

        RenderResourceView<RenderTarget> CreateViewForSwapChainBuffer(
            const SwapChainBuffer* buffer,
            const D3D12_RENDER_TARGET_VIEW_DESC* desc = nullptr) const
        {
            RenderResourceView<RenderTarget> textureView;
            textureView.heap = GetRenderTargetDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.handle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            d3d12Device->CreateRenderTargetView(buffer->GetResource(), desc, textureView.handle);
            return textureView;
        }

        RenderResourceView<UnorderedAccess> CreateUAVViewForTexture(
            const RenderTextureBase* resource,
            const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc = nullptr,
            ID3D12Resource* counterResource = nullptr) const
        {
            RenderResourceView<UnorderedAccess> view;
            view.heap = GetUnorderedAccessDescriptorHeap();
            view.heap->AllocDescriptorHandle(view.cpuHandle, view.gpuHandle);
            view.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            view.cachedCounterResource = counterResource;
            d3d12Device->CreateUnorderedAccessView(resource->GetResource(), counterResource, desc, view.cpuHandle);
            return view;
        }

    private:
        const GraphicsDevice* graphicsDevice;
        Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device;

        std::unique_ptr<DescriptorHeap> rtvDescriptorHeap = nullptr;
        std::unique_ptr<DescriptorHeap> dsvDescriptorHeap = nullptr;
        std::unique_ptr<DescriptorHeap> sharedDescriptorHeap = nullptr;
    };
}

#endif //VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H
