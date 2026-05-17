//
// Created by Natsurainko on 2026/5/17.
//

#ifndef VERTIX_DESCRIPTORHEAPHANDLE_H
#define VERTIX_DESCRIPTORHEAPHANDLE_H

#include <cstdint>
#include <xhash>

#include <d3d12/d3d12.h>

namespace Vertix {
    struct DescriptorHeapHandle {
        uint32_t slot = {};

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
        ID3D12DescriptorHeap* heap = nullptr;

        DescriptorHeapHandle& operator=(const DescriptorHeapHandle&) = default;
        explicit operator bool() const noexcept { return slot; }
        bool operator == (const DescriptorHeapHandle& other) const noexcept { return slot == other.slot; }
        bool operator <  (const DescriptorHeapHandle& other) const noexcept { return slot < other.slot; }
    };
}

template<>
struct std::hash<Vertix::DescriptorHeapHandle> {
    std::size_t operator()(const Vertix::DescriptorHeapHandle& handle) const noexcept {
        return handle.slot;
    }
};

#endif //VERTIX_DESCRIPTORHEAPHANDLE_H
