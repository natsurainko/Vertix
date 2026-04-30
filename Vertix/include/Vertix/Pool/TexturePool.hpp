//
// Created by Natsurainko on 2026/3/17.
//

#ifndef VERTIX_TEXTUREPOOL_HPP
#define VERTIX_TEXTUREPOOL_HPP

#include <d3d12/d3dx12_root_signature.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Pool/ResourcePool.hpp"
#include "Vertix/Primitive/Texture.h"

namespace Vertix {
    template<uint32_t Capacity = 1024>
    class TexturePool : public ResourcePool<Texture, TextureHandle, Capacity> {
    public:
        explicit TexturePool(const GraphicsDevice* graphicsDevice) {
            d3d12Device = graphicsDevice->GetD3D12Device();
            const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC {
                .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                .NumDescriptors = Capacity,
                .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            };

            ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap)));
            descriptorLength = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            heapStartCpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
            heapStartGpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
        }

        [[nodiscard]]
        TextureHandle AllocateNamed(
            const std::wstring &name,
            std::unique_ptr<Texture> resource = nullptr) noexcept
        {
            const TextureHandle handle = ResourcePool<Texture, TextureHandle, Capacity>::Allocate(std::move(resource));
            NameResource(handle, name);
            return handle;
        }

        void Free(const TextureHandle handle) noexcept override {
            namedResources.erase(handle);
            ResourcePool<Texture, TextureHandle, Capacity>::Free(handle);
        }

        void NameResource(const TextureHandle handle, const std::wstring &name) noexcept {
            namedResources[handle] = name;
            namedResourceHandles[name] = handle;
        }

        [[nodiscard]]
        bool ContainsNamedResource(const std::wstring &name) const noexcept {
            return namedResourceHandles.contains(name);
        }

        [[nodiscard]]
        TextureHandle GetNamedHandle(const std::wstring &name) noexcept {
            return namedResourceHandles[name];
        }

        [[nodiscard]]
        const std::wstring& GetHandleName(const TextureHandle handle) noexcept {
            return namedResources[handle];
        }

        [[nodiscard]]
        D3D12_RESOURCE_DESC GetResourceDesc(const TextureHandle handle) const noexcept {
            auto* texture = this->template GetAs<Texture>(handle);
            return texture->GetResource()->GetDesc();
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap() const noexcept {
            return descriptorHeap;
        }

        [[nodiscard]]
        D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(const TextureHandle handle) const noexcept {
            const uint32_t index = handle.slot - 1;
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, static_cast<INT>(index), descriptorLength);
        }

    private:
        UINT descriptorLength;
        D3D12_CPU_DESCRIPTOR_HANDLE heapStartCpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE heapStartGpuHandle{};
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

        std::unordered_map<TextureHandle, std::wstring> namedResources{};
        std::unordered_map<std::wstring, TextureHandle> namedResourceHandles{};

        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
    };
}

#endif //VERTIX_TEXTUREPOOL_HPP
