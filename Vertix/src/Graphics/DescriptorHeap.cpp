//
// Created by Natsurainko on 2026/1/29.
//

#include "Vertix/Graphics/DescriptorHeap.h"

#include <cassert>
#include <memory>
#include <optional>
#include <d3d12/d3dx12_root_signature.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Graphics/DescriptorRange.h"

Vertix::DescriptorHeap::DescriptorHeap(
    const GraphicsDevice* graphicsDevice,
    const D3D12_DESCRIPTOR_HEAP_TYPE heapType,
    const uint32_t maxDescriptors,
    const bool shaderVisible) : DescriptorHeap(graphicsDevice->GetD3D12Device().Get(), heapType, maxDescriptors, shaderVisible) {}

Vertix::DescriptorHeap::DescriptorHeap(
    ID3D12Device* d3d12Device,
    const D3D12_DESCRIPTOR_HEAP_TYPE heapType,
    const uint32_t maxDescriptors,
    const bool shaderVisible) : maxDescriptors(maxDescriptors), shaderVisible(shaderVisible)
{
    auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC { .Type = heapType, .NumDescriptors = maxDescriptors };
    if (shaderVisible) heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ThrowIfFailed(d3d12Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap)));
    descriptorLength = d3d12Device->GetDescriptorHandleIncrementSize(heapType);
    heapStartCpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    if (shaderVisible) {
        heapStartGpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    for (uint32_t i = 0; i < maxDescriptors; ++i) {
        freeSlots.insert(i);
    }
}

Vertix::DescriptorHeapHandle Vertix::DescriptorHeap::AllocDescriptorHandle() {
    assert(!IsFull() && "The descriptor heap is full.");

    const auto it = freeSlots.begin();
    const uint32_t slot = *it;
    freeSlots.erase(it);

    DescriptorHeapHandle handle {
        .slot      = slot,
        .cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, static_cast<INT>(slot), descriptorLength),
        .heap      = descriptorHeap.Get()
    };

    if (shaderVisible) handle.gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heapStartGpuHandle, static_cast<INT>(slot), descriptorLength);

    return handle;
}

void Vertix::DescriptorHeap::AllocDescriptorHandles(const uint32_t count, DescriptorHeapHandle* handles) {
    assert(!IsFull() && "The descriptor heap is full.");
    assert(freeSlots.size() >= count && "Not enough free descriptors in heap.");

    for (uint32_t i = 0; i < count; ++i) {
        const auto it = freeSlots.begin();
        const uint32_t slot = *it;
        freeSlots.erase(it);

        handles[i] = DescriptorHeapHandle {
            .slot      = slot,
            .cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heapStartCpuHandle, static_cast<INT>(slot), descriptorLength),
            .heap      = descriptorHeap.Get()
        };

        if (shaderVisible) handles[i].gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heapStartGpuHandle, static_cast<INT>(slot), descriptorLength);
    }
}

std::unique_ptr<Vertix::DescriptorRange> Vertix::DescriptorHeap::AllocateRange(const uint32_t count) {
    assert(freeSlots.size() >= count && "Not enough free descriptors.");

    uint32_t startSlot = 0, slotCount = 0;
    std::optional<UINT> prev;
    for (const uint32_t slot : freeSlots) {
        if (prev.has_value() && slot == *prev + 1) ++slotCount;
        else { startSlot = slot; slotCount = 1; }
        if (slotCount == count) break;
        prev = slot;
    }

    assert(slotCount == count && "No contiguous block of sufficient size.");

    for (uint32_t i = startSlot; i < startSlot + count; ++i)
        freeSlots.erase(i);

    return std::unique_ptr<DescriptorRange>(new DescriptorRange(this, startSlot, slotCount));
}

void Vertix::DescriptorHeap::FreeRange(DescriptorRange* range) {
    assert(range->parentHeap == this);

    for (const auto slot : range->freeSlots) {
        freeSlots.insert(slot);
    }

    range->parentHeap = nullptr;
}

void Vertix::DescriptorHeap::FreeDescriptorHandle(const DescriptorHeapHandle &handle) {
    freeSlots.insert(handle.slot);
}

void Vertix::DescriptorHeap::FreeDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE handle) {
    freeSlots.insert(GetIndexOfDescriptorHandle(handle));
}

void Vertix::DescriptorHeap::FreeDescriptorHandles(const uint32_t count, const DescriptorHeapHandle* handles) {
    for (uint32_t i = 0; i < count; ++i) {
        freeSlots.insert(handles[i].slot);
    }
}

UINT Vertix::DescriptorHeap::GetIndexOfDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE handle) const {
    const UINT64 offset = handle.ptr - heapStartCpuHandle.ptr;
    assert(!(offset % descriptorLength) && "Invalid descriptor handle");
    const UINT index = static_cast<UINT>(offset / descriptorLength);
    assert(index < maxDescriptors && "The descriptor handle does not belong to this heap.");
    return index;
}
