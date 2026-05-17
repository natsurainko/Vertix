//
// Created by Natsurainko on 2026/5/17.
//

#include "Vertix/Graphics/DescriptorRange.h"

#include <cassert>
#include <d3d12/d3dx12_root_signature.h>

#include "Vertix/Graphics/DescriptorHeap.h"

Vertix::DescriptorRange::DescriptorRange(
    DescriptorHeap* parentHeap,
    const uint32_t startSlot,
    const uint32_t slotCount)
: startSlot(startSlot), slotCount(slotCount), parentHeap(parentHeap)
{
    descriptorLength = parentHeap->GetDescriptorLength();
    shaderVisible = parentHeap->IsShaderVisible();

    for (uint32_t i = startSlot; i < startSlot + slotCount; ++i)
        freeSlots.insert(i);
}

Vertix::DescriptorRange::~DescriptorRange() {
    if (parentHeap) {
        parentHeap->FreeRange(this);
    }
}

Vertix::DescriptorHeapHandle Vertix::DescriptorRange::AllocDescriptorHandle() {
    assert(!IsFull() && "The descriptor range is full.");

    const auto it = freeSlots.begin();
    const uint32_t slot = *it;
    freeSlots.erase(it);

    DescriptorHeapHandle handle {
        .slot      = slot,
        .cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(parentHeap->GetCpuDescriptorHandleForHeapStart(), static_cast<INT>(slot), descriptorLength),
        .heap      = parentHeap->GetDescriptorHeap()
    };

    if (shaderVisible) handle.gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(parentHeap->GetGpuDescriptorHandleForHeapStart(), static_cast<INT>(slot), descriptorLength);

    return handle;
}

void Vertix::DescriptorRange::AllocDescriptorHandles(const UINT count, DescriptorHeapHandle* handles) {
    assert(!IsFull() && "The descriptor range is full.");
    assert(freeSlots.size() >= count && "Not enough free descriptors in range.");

    for (UINT i = 0; i < count; ++i) {
        const auto it = freeSlots.begin();
        const uint32_t slot = *it;
        freeSlots.erase(it);

        handles[i] = DescriptorHeapHandle {
            .slot      = slot,
            .cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(parentHeap->GetCpuDescriptorHandleForHeapStart(), static_cast<INT>(slot), descriptorLength),
            .heap      = parentHeap->GetDescriptorHeap()
        };

        if (shaderVisible) handles[i].gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(parentHeap->GetGpuDescriptorHandleForHeapStart(), static_cast<INT>(slot), descriptorLength);
    }
}

void Vertix::DescriptorRange::FreeDescriptorHandle(const DescriptorHeapHandle &handle) {
    freeSlots.insert(handle.slot);
}

void Vertix::DescriptorRange::FreeDescriptorHandles(const UINT count, const DescriptorHeapHandle *handles) {
    for (UINT i = 0; i < count; ++i) {
        freeSlots.insert(handles[i].slot);
    }
}
