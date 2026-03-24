//
// Created by Natsurainko on 2026/1/29.
//

#ifndef VERTIX_DESCRIPTORHEAP_H
#define VERTIX_DESCRIPTORHEAP_H

#include <set>

#include "GraphicsDevice.h"
#include "VERTIX_EXPORT.h"

namespace Vertix {
    class VERTIX_API DescriptorHeap {
    public:
        explicit DescriptorHeap(
            const GraphicsDevice* graphicsDevice,
            D3D12_DESCRIPTOR_HEAP_TYPE heapType,
            UINT maxDescriptors = 16,
            bool shaderVisible = false);

        void AllocDescriptorHandle(
            D3D12_CPU_DESCRIPTOR_HANDLE &handle,
            UINT* indexPtr = nullptr);

        void AllocDescriptorHandle(
            D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle,
            D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle,
            UINT* indexPtr = nullptr);

        void AllocDescriptorHandle(
            D3D12_CPU_DESCRIPTOR_HANDLE* handles,
            UINT numHandles,
            UINT* indicesPtr = nullptr);

        void AllocDescriptorHandle(
            D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandles,
            D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandles,
            UINT numHandles,
            UINT* indicesPtr = nullptr);

        void FreeDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);

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
        UINT GetIndexOfDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;
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