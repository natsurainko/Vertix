//
// Created by Natsurainko on 2026/1/29.
//

#ifndef VERTIX_DESCRIPTORHEAP_H
#define VERTIX_DESCRIPTORHEAP_H

#include <memory>
#include <set>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Graphics/DescriptorRange.h"
#include "Vertix/Graphics/GraphicsDevice.h"

namespace Vertix {
    class DescriptorHeap : public IDescriptorAllocator {
    public:
        VERTIX_API explicit DescriptorHeap(
            const GraphicsDevice* graphicsDevice,
            D3D12_DESCRIPTOR_HEAP_TYPE heapType,
            uint32_t maxDescriptors = 16,
            bool shaderVisible = false);

        VERTIX_API explicit DescriptorHeap(
            ID3D12Device* d3d12Device,
            D3D12_DESCRIPTOR_HEAP_TYPE heapType,
            uint32_t maxDescriptors = 16,
            bool shaderVisible = false);

        VERTIX_API DescriptorHeapHandle AllocDescriptorHandle() override;
        VERTIX_API void AllocDescriptorHandles(uint32_t count, DescriptorHeapHandle* handles);

        VERTIX_API void FreeDescriptorHandle(const DescriptorHeapHandle &handle) override;
        VERTIX_API void FreeDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);
        VERTIX_API void FreeDescriptorHandles(uint32_t count, const DescriptorHeapHandle* handles);

        VERTIX_API std::unique_ptr<DescriptorRange> AllocateRange(UINT count);
        VERTIX_API void FreeRange(DescriptorRange* range);

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandleForHeapStart() const noexcept { return heapStartCpuHandle; }
        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandleForHeapStart() const noexcept { return heapStartGpuHandle; }
        [[nodiscard]] uint32_t              GetDescriptorLength() const noexcept { return descriptorLength; }
        [[nodiscard]] ID3D12DescriptorHeap* GetDescriptorHeap()   const noexcept { return descriptorHeap.Get(); }
        [[nodiscard]] ID3D12DescriptorHeap* const* GetDescriptorHeapAddress() const noexcept { return descriptorHeap.GetAddressOf(); }
        [[nodiscard]] bool IsShaderVisible() const noexcept { return shaderVisible; }

        [[nodiscard]] bool     IsFull() const noexcept override { return freeSlots.empty(); }
        [[nodiscard]] uint32_t GetFreeCount() const noexcept override { return static_cast<uint32_t>(freeSlots.size()); }
        [[nodiscard]] uint32_t GetTotalCount() const noexcept override { return maxDescriptors; }
        [[nodiscard]] uint32_t GetStartSlot() const noexcept override { return 0; }
    private:
        uint32_t maxDescriptors;
        uint32_t descriptorLength;
        bool shaderVisible;

        D3D12_CPU_DESCRIPTOR_HANDLE heapStartCpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE heapStartGpuHandle{};
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
        std::set<uint32_t> freeSlots;

        UINT GetIndexOfDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;
    };
}

#endif //VERTIX_DESCRIPTORHEAP_H
