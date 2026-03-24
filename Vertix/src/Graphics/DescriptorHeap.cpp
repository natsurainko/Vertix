//
// Created by Natsurainko on 2026/1/29.
//

#include "Graphics/DescriptorHeap.h"

#include <cassert>
#include <exception>

#include "d3d12/d3dx12_root_signature.h"
#include "Exceptions/HResultException.h"

Vertix::DescriptorHeap::DescriptorHeap(
    const GraphicsDevice *graphicsDevice,
    const D3D12_DESCRIPTOR_HEAP_TYPE heapType,
    const UINT maxDescriptors,
    const bool shaderVisible) : maxDescriptors(maxDescriptors), shaderVisible(shaderVisible)
{
    const auto &d3d12Device = graphicsDevice->GetD3D12Device();
    auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC { .Type = heapType, .NumDescriptors = maxDescriptors };
    if (shaderVisible) heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap)));
    descriptorLength = d3d12Device->GetDescriptorHandleIncrementSize(heapType);
    heapStartCpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    if (shaderVisible) {
        heapStartGpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    for (UINT i = 0; i < maxDescriptors; ++i) {
        freeIndices.insert(i);
    }
}

void Vertix::DescriptorHeap::AllocDescriptorHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE &handle,
    UINT* indexPtr)
{
    assert(!IsFull() && "The descriptor heap is full.");
    assert(!shaderVisible && "This is a GPU-visible descriptor heap; both CPU and GPU handles must be allocated simultaneously.");

    const auto it = freeIndices.begin();
    const UINT index = *it;
    freeIndices.erase(it);

    handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, index, descriptorLength);
    if (indexPtr) {
        *indexPtr = index;
    }
}

void Vertix::DescriptorHeap::AllocDescriptorHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE &cpuHandle,
    D3D12_GPU_DESCRIPTOR_HANDLE &gpuHandle,
    UINT* indexPtr)
{
    assert(!IsFull() && "The descriptor heap is full.");
    assert(shaderVisible && "This is a GPU-invisible descriptor heap, which can only allocate CPU handles.");

    const auto it = freeIndices.begin();
    const UINT index = *it;
    freeIndices.erase(it);

    cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, index, descriptorLength);
    gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heapStartGpuHandle, index, descriptorLength);

    if (indexPtr) {
        *indexPtr = index;
    }
}

void Vertix::DescriptorHeap::AllocDescriptorHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE* handles,
    const UINT numHandles,
    UINT* indicesPtr)
{
    assert(!IsFull() && "The descriptor heap is full.");
    assert(!shaderVisible && "This is a GPU-visible descriptor heap; both CPU and GPU handles must be allocated simultaneously.");
    assert(freeIndices.size() >= numHandles && "Not enough free descriptors in heap.");

    if (indicesPtr) {
        for (UINT i = 0; i < numHandles; i++) {
            const auto it = freeIndices.begin();
            const UINT index = *it;
            freeIndices.erase(it);

            handles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, index, descriptorLength);
            indicesPtr[i] = index;
        }
        return;
    }

    for (UINT i = 0; i < numHandles; i++) {
        const auto it = freeIndices.begin();
        const UINT index = *it;
        freeIndices.erase(it);

        handles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, index, descriptorLength);
    }
}

void Vertix::DescriptorHeap::AllocDescriptorHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandles,
    D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandles,
    const UINT numHandles,
    UINT* indicesPtr)
{
    assert(!IsFull() && "The descriptor heap is full.");
    assert(shaderVisible && "This is a GPU-invisible descriptor heap, which can only allocate CPU handles.");
    assert(freeIndices.size() >= numHandles && "Not enough free descriptors in heap.");

    if (indicesPtr) {
        for (UINT i = 0; i < numHandles; i++) {
            const auto it = freeIndices.begin();
            const UINT index = *it;
            freeIndices.erase(it);

            cpuHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, index, descriptorLength);
            gpuHandles[i] = CD3DX12_GPU_DESCRIPTOR_HANDLE(heapStartGpuHandle, index, descriptorLength);
            indicesPtr[i] = index;
        }
        return;
    }

    for (UINT i = 0; i < numHandles; i++) {
        const auto it = freeIndices.begin();
        const UINT index = *it;
        freeIndices.erase(it);

        cpuHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, index, descriptorLength);
        gpuHandles[i] = CD3DX12_GPU_DESCRIPTOR_HANDLE(heapStartGpuHandle, index, descriptorLength);
    }
}

UINT Vertix::DescriptorHeap::GetIndexOfDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE handle) const {
    const UINT64 offset = handle.ptr - heapStartCpuHandle.ptr;
    assert(!(offset % descriptorLength) && "Invalid descriptor handle");
    const UINT index = static_cast<UINT>(offset / descriptorLength);
    assert(index < maxDescriptors && "The descriptor handle does not belong to this heap.");
    return index;
}

void Vertix::DescriptorHeap::FreeDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE handle) {
    freeIndices.insert(GetIndexOfDescriptorHandle(handle));
}
