//
// Created by Natsurainko on 2026/1/29.
//

#ifndef VERTIX_DESCRIPTORHEAP_H
#define VERTIX_DESCRIPTORHEAP_H

#include <set>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Graphics/GraphicsDevice.h"

namespace Vertix {
    class DescriptorHeap {
    public:
        VERTIX_API explicit DescriptorHeap(
            const GraphicsDevice* graphicsDevice,
            D3D12_DESCRIPTOR_HEAP_TYPE heapType,
            UINT maxDescriptors = 16,
            bool shaderVisible = false);

        VERTIX_API explicit DescriptorHeap(
            ID3D12Device* d3d12Device,
            D3D12_DESCRIPTOR_HEAP_TYPE heapType,
            UINT maxDescriptors = 16,
            bool shaderVisible = false);

        VERTIX_API void AllocDescriptorHandle(
            D3D12_CPU_DESCRIPTOR_HANDLE &handle,
            UINT* indexPtr = nullptr);

        VERTIX_API void AllocDescriptorHandle(
            D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle,
            D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle,
            UINT* indexPtr = nullptr);

        VERTIX_API void AllocDescriptorHandle(
            D3D12_CPU_DESCRIPTOR_HANDLE* handles,
            UINT numHandles,
            UINT* indicesPtr = nullptr);

        VERTIX_API void AllocDescriptorHandle(
            D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandles,
            D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandles,
            UINT numHandles,
            UINT* indicesPtr = nullptr);

        VERTIX_API void FreeDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap() const noexcept {
            return descriptorHeap;
        }

        [[nodiscard]]
        bool IsFull() const noexcept {
            return freeIndices.empty();
        }

        [[nodiscard]]
        D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandleForHeapStart() const noexcept {
            return heapStartCpuHandle;
        }

        [[nodiscard]]
        D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandleForHeapStart() const noexcept {
            return heapStartGpuHandle;
        }

        [[nodiscard]]
        UINT GetDescriptorLength() const noexcept {
            return descriptorLength;
        }

        [[nodiscard]]
        VERTIX_API UINT GetIndexOfDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;
    private:
        UINT maxDescriptors;
        UINT descriptorLength;
        bool shaderVisible;

        D3D12_CPU_DESCRIPTOR_HANDLE heapStartCpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE heapStartGpuHandle{};
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
        std::set<UINT> freeIndices;
    };
}

#endif //VERTIX_DESCRIPTORHEAP_H
