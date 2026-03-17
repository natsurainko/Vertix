//
// Created by Natsurainko on 2026/3/17.
//

#ifndef VERTIX_TEXTUREPOOL_HPP
#define VERTIX_TEXTUREPOOL_HPP

#include <d3dx12_root_signature.h>
#include <d3dx12_barriers.h>

#include "ResourcePool.hpp"
#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsCommandList.h"
#include "Primitive/Texture.h"

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

        D3D12_CPU_DESCRIPTOR_HANDLE CreateShaderResourceView(
            TextureHandle handle,
            const D3D12_SHADER_RESOURCE_VIEW_DESC &srvDesc)
        {
            const auto descriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, handle.slot, descriptorLength);
            d3d12Device->CreateShaderResourceView(
                this->slots[handle.slot].get()->GetResource().Get(),
                &srvDesc,
                descriptorHandle
            );
            return descriptorHandle;
        }

        void NameResource(const TextureHandle handle, const std::wstring &name) {
            namedResources[handle] = name;
            namedResourceHandles[name] = handle;
        }

        [[nodiscard]]
        bool ContainsNamedResource(const std::wstring &name) const {
            return namedResourceHandles.contains(name);
        }

        [[nodiscard]]
        TextureHandle GetNamedHandle(const std::wstring &name) {
            return namedResourceHandles[name];
        }

        void MarkBarrier(
            const TextureHandle handle,
            const D3D12_RESOURCE_STATES stateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
            const D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
        {
            std::lock_guard lock(barrierMutex);
            barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(
                this->slots[handle.slot].get()->GetResource().Get(),
                stateBefore, stateAfter
            ));
            needFlushBarrier.store(true, std::memory_order_release);
        }

        void FlushBarriers(const GraphicsCommandList* graphicsCommandList) {
            if (!needFlushBarrier.load(std::memory_order_acquire))
                return;

            std::vector<D3D12_RESOURCE_BARRIER> local;
            {
                std::lock_guard lock(barrierMutex);
                std::swap(local, barriers);
                needFlushBarrier.store(false, std::memory_order_relaxed);
            }

            if (!local.empty()) {
                graphicsCommandList->GetD3D12GraphicsCommandList()->ResourceBarrier(static_cast<UINT>(local.size()), local.data());
            }
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap() const noexcept {
            return descriptorHeap;
        }

    private:
        std::atomic<bool> needFlushBarrier = false;
        std::mutex barrierMutex;
        std::vector<D3D12_RESOURCE_BARRIER> barriers{};

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
