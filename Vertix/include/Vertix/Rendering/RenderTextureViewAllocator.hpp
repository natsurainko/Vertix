//
// Created by Natsurainko on 2026/4/30.
//

#ifndef VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H
#define VERTIX_RENDERTEXTUREDESCRIPTORHEAP_H

#include <memory>

#include "RenderTextureAccessor.h"
#include "RenderTextureView.h"
#include "RenderTexture.hpp"
#include "Vertix/Graphics/DescriptorHeap.h"

namespace Vertix {
    class RenderTextureViewAllocator {
    public:
        explicit RenderTextureViewAllocator(const GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice), d3d12Device(graphicsDevice->GetD3D12Device()) {}

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

        template<RenderTextureAccessor TextureAccessor> requires ((TextureAccessor & RenderTarget) != 0)
        RenderTextureView<RenderTarget> CreateRenderTargetView(
            const RenderTexture<TextureAccessor>* texture,
            const D3D12_RENDER_TARGET_VIEW_DESC* desc = nullptr)
        {
            RenderTextureView<RenderTarget> textureView;
            textureView.heap = GetRenderTargetDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.handle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            d3d12Device->CreateRenderTargetView(texture->GetResource(), desc, textureView.handle);
            return textureView;
        }

        template<RenderTextureAccessor TextureAccessor> requires ((TextureAccessor & DepthStencil) != 0)
        RenderTextureView<DepthStencil> CreateDepthStencilView(
            const RenderTexture<TextureAccessor>* texture,
            const D3D12_DEPTH_STENCIL_VIEW_DESC* desc = nullptr)
        {
            RenderTextureView<DepthStencil> textureView;
            textureView.heap = GetDepthStencilDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.handle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            d3d12Device->CreateDepthStencilView(texture->GetResource(), desc, textureView.handle);
            return textureView;
        }

        template<RenderTextureAccessor TextureAccessor> requires ((TextureAccessor & UnorderedAccess) != 0)
        RenderTextureView<UnorderedAccess> CreateUnorderedAccessView(
            const RenderTexture<TextureAccessor>* texture,
            const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc = nullptr,
            ID3D12Resource* counterResource = nullptr)
        {
            RenderTextureView<UnorderedAccess> textureView;
            textureView.heap = GetUnorderedAccessDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.cpuHandle, textureView.gpuHandle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            textureView.cachedCounterResource = counterResource;
            d3d12Device->CreateUnorderedAccessView(texture->GetResource(), counterResource, desc, textureView.cpuHandle);
            return textureView;
        }

        template<RenderTextureAccessor TextureAccessor> requires ((TextureAccessor & ShaderResource) != 0)
        RenderTextureView<ShaderResource> CreateShaderResourceView(
            const RenderTexture<TextureAccessor>* texture,
            const D3D12_SHADER_RESOURCE_VIEW_DESC* desc = nullptr)
        {
            RenderTextureView<ShaderResource> textureView;
            textureView.heap = GetShaderResourceDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.cpuHandle, textureView.gpuHandle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            d3d12Device->CreateShaderResourceView(texture->GetResource(), desc, textureView.cpuHandle);
            return textureView;
        }

        RenderTextureView<RenderTarget> CreateRenderTargetView(
            const RenderTextureBase* texture,
            const D3D12_RENDER_TARGET_VIEW_DESC* desc = nullptr) const
        {
            RenderTextureView<RenderTarget> textureView;
            textureView.heap = GetRenderTargetDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.handle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            d3d12Device->CreateRenderTargetView(texture->GetResource(), desc, textureView.handle);
            return textureView;
        }

        RenderTextureView<DepthStencil> CreateDepthStencilView(
            const RenderTextureBase* texture,
            const D3D12_DEPTH_STENCIL_VIEW_DESC* desc = nullptr) const
        {
            RenderTextureView<DepthStencil> textureView;
            textureView.heap = GetDepthStencilDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.handle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            d3d12Device->CreateDepthStencilView(texture->GetResource(), desc, textureView.handle);
            return textureView;
        }

        RenderTextureView<UnorderedAccess> CreateUnorderedAccessView(
            const RenderTextureBase* texture,
            const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc = nullptr,
            ID3D12Resource* counterResource = nullptr) const
        {
            RenderTextureView<UnorderedAccess> textureView;
            textureView.heap = GetUnorderedAccessDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.cpuHandle, textureView.gpuHandle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            textureView.cachedCounterResource = counterResource;
            d3d12Device->CreateUnorderedAccessView(texture->GetResource(), counterResource, desc, textureView.cpuHandle);
            return textureView;
        }

        RenderTextureView<ShaderResource> CreateShaderResourceView(
            const RenderTextureBase* texture,
            const D3D12_SHADER_RESOURCE_VIEW_DESC* desc = nullptr) const
        {
            RenderTextureView<ShaderResource> textureView;
            textureView.heap = GetShaderResourceDescriptorHeap();
            textureView.heap->AllocDescriptorHandle(textureView.cpuHandle, textureView.gpuHandle);
            textureView.cachedDesc = desc ? std::make_optional(*desc) : std::nullopt;
            d3d12Device->CreateShaderResourceView(texture->GetResource(), desc, textureView.cpuHandle);
            return textureView;
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
