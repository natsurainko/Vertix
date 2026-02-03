//
// Created by Natsurainko on 2026/1/29.
//

#include "Graphics/DescriptorHeap.h"

#include <exception>

#include "Exceptions/HResultException.h"

Vertix::DescriptorHeap::DescriptorHeap(const GraphicsDevice *graphicsDevice,
                                       const D3D12_DESCRIPTOR_HEAP_TYPE heapType,
                                       const UINT maxDescriptors,
                                       const bool shaderVisible) : maxDescriptors(maxDescriptors), shaderVisible(shaderVisible) {
    const auto &d3d12Device = graphicsDevice->GetD3D12Device();
    auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC { .Type = heapType, .NumDescriptors = maxDescriptors };
    if (shaderVisible) heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap)));
    descriptorLength = d3d12Device->GetDescriptorHandleIncrementSize(heapType);
    currentAvailableCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart());
    if (shaderVisible) currentAvailableGpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

void Vertix::DescriptorHeap::AllocDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE &handle) {
    if (currentHandles == maxDescriptors)
        throw std::exception("The descriptor heap is full.");
    if (shaderVisible)
        throw std::exception("This is a GPU-visible descriptor heap; both CPU and GPU handles must be allocated simultaneously.");

    handle = currentAvailableCpuHandle;
    currentAvailableCpuHandle.Offset(1, descriptorLength);
    currentHandles++;
}

void Vertix::DescriptorHeap::AllocDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE &cpuHandle,
                                                   CD3DX12_GPU_DESCRIPTOR_HANDLE &gpuHandle) {
    if (currentHandles == maxDescriptors)
        throw std::exception("The descriptor heap is full.");
    if (!shaderVisible)
        throw std::exception("This is a GPU-invisible descriptor heap, which can only allocate CPU handles.");

    cpuHandle = currentAvailableCpuHandle;
    gpuHandle = currentAvailableGpuHandle;
    currentAvailableCpuHandle.Offset(1, descriptorLength);
    currentAvailableGpuHandle.Offset(1, descriptorLength);
    currentHandles++;
}

void Vertix::DescriptorHeap::AllocDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE* handles, const UINT numHandles) {
    if (currentHandles == maxDescriptors)
        throw std::exception("The descriptor heap is full.");
    if (shaderVisible)
        throw std::exception("This is a GPU-visible descriptor heap; both CPU and GPU handles must be allocated simultaneously.");

    for (UINT i = 0; i < numHandles; i++) {
        *handles++ = currentAvailableCpuHandle;
        currentAvailableCpuHandle.Offset(1, descriptorLength);
        currentHandles++;
    }
}

void Vertix::DescriptorHeap::AllocDescriptorHandle(CD3DX12_CPU_DESCRIPTOR_HANDLE* cpuHandles,
                                                   CD3DX12_GPU_DESCRIPTOR_HANDLE* gpuHandles,
                                                   const UINT numHandles) {
    if (currentHandles == maxDescriptors)
        throw std::exception("The descriptor heap is full.");
    if (!shaderVisible)
        throw std::exception("This is a GPU-invisible descriptor heap, which can only allocate CPU handles.");

    for (UINT i = 0; i < numHandles; i++) {
        *cpuHandles++ = currentAvailableCpuHandle;
        *gpuHandles++ = currentAvailableGpuHandle;
        currentAvailableCpuHandle.Offset(1, descriptorLength);
        currentAvailableGpuHandle.Offset(1, descriptorLength);
        currentHandles++;
    }
}
