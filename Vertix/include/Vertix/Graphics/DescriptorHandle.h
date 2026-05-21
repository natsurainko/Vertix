//
// Created by Natsurainko on 2026/5/17.
//

#ifndef VERTIX_DESCRIPTORHEAPHANDLE_H
#define VERTIX_DESCRIPTORHEAPHANDLE_H

#include <cstdint>
#include <optional>
#include <xhash>

#include <d3d12/d3d12.h>

namespace Vertix {
    struct DescriptorHandle {
        uint32_t slot = {};

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
        ID3D12DescriptorHeap* heap = nullptr;

        DescriptorHandle& operator=(const DescriptorHandle&) = default;
        explicit operator bool() const noexcept { return slot; }
        bool operator == (const DescriptorHandle& other) const noexcept { return slot == other.slot; }
        bool operator <  (const DescriptorHandle& other) const noexcept { return slot < other.slot; }

        void CreateRTV(
            ID3D12Device* device,
            ID3D12Resource* resource,
            const std::optional<D3D12_RENDER_TARGET_VIEW_DESC> &desc = std::nullopt) const
        {
            device->CreateRenderTargetView(resource, desc.has_value() ? &desc.value() : nullptr, cpuHandle);
        }

        void CreateDSV(
            ID3D12Device* device,
            ID3D12Resource* resource,
            const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC> &desc = std::nullopt) const
        {
            device->CreateDepthStencilView(resource, desc.has_value() ? &desc.value() : nullptr, cpuHandle);
        }

        void CreateCBV(
            ID3D12Device* device,
            const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) const
        {
            device->CreateConstantBufferView(&desc, cpuHandle);
        }

        void CreateUAV(
            ID3D12Device* device,
            ID3D12Resource* resource,
            const std::optional<D3D12_UNORDERED_ACCESS_VIEW_DESC> &desc = std::nullopt,
            ID3D12Resource* counterResource = nullptr) const
        {
            device->CreateUnorderedAccessView(resource, counterResource, desc.has_value() ? &desc.value() : nullptr, cpuHandle);
        }

        void CreateSRV(
            ID3D12Device* device,
            ID3D12Resource* resource,
            const std::optional<D3D12_SHADER_RESOURCE_VIEW_DESC> &desc = std::nullopt) const
        {
            device->CreateShaderResourceView(resource, desc.has_value() ? &desc.value() : nullptr, cpuHandle);
        }
    };
}

template<>
struct std::hash<Vertix::DescriptorHandle> {
    std::size_t operator()(const Vertix::DescriptorHandle& handle) const noexcept {
        return handle.slot;
    }
};

#endif //VERTIX_DESCRIPTORHEAPHANDLE_H
