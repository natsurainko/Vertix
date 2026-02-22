//
// Created by Natsurainko on 2026/1/29.
//

#ifndef VERTIX_DESCRIPTORHEAP_H
#define VERTIX_DESCRIPTORHEAP_H

#include <d3d12/d3dx12_root_signature.h>

#include "GraphicsDevice.h"

namespace Vertix {
    class DescriptorHeap {
    public:
        explicit DescriptorHeap(const GraphicsDevice* graphicsDevice,
                                D3D12_DESCRIPTOR_HEAP_TYPE heapType,
                                UINT maxDescriptors = 16,
                                bool shaderVisible = false);

        void AllocDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE &handle);
        void AllocDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE &cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE &gpuHandle);

        void AllocDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE* handles, UINT numHandles);
        void AllocDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE* cpuHandles,
                                   CD3DX12_GPU_DESCRIPTOR_HANDLE* gpuHandles,
                                   UINT numHandles);

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap() const {
            return descriptorHeap;
        }

        [[nodiscard]]
        bool IsFull() const {
            return currentHandles == maxDescriptors;
        }

        [[nodiscard]]
        UINT GetCurrentHandleCount() const {
            return currentHandles;
        }

        [[nodiscard]]
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandleForHeapStart() const {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart());
        }

        [[nodiscard]]
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandleForHeapStart() const {
            return CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart());
        }

    private:
        UINT maxDescriptors;
        UINT descriptorLength;
        UINT currentHandles = 0;
        bool shaderVisible;

        CD3DX12_CPU_DESCRIPTOR_HANDLE currentAvailableCpuHandle{};
        CD3DX12_GPU_DESCRIPTOR_HANDLE currentAvailableGpuHandle{};
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    };
}

#endif //VERTIX_DESCRIPTORHEAP_H