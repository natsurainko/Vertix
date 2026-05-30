//
// Created by Natsurainko on 2026/5/19.
//

#pragma once

#include <memory>
#include <d3d12/d3d12.h>

#include "DescriptorHeap.h"

namespace Vertix {
    class DescriptorHeapSet {
        ID3D12Device*         device;
        ID3D12DescriptorHeap* gpuHeaps[2] = {};

        std::unique_ptr<DescriptorHeap> descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};

    public:
        VERTIX_API explicit DescriptorHeapSet(
            ID3D12Device*  device,
            const uint32_t heapsCapacity[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]);

        [[nodiscard]] VERTIX_API DescriptorHandle CreateRTV(
            ID3D12Resource*                                     resource,
            const std::optional<D3D12_RENDER_TARGET_VIEW_DESC> &desc = std::nullopt) const;

        [[nodiscard]] VERTIX_API DescriptorHandle CreateDSV(
            ID3D12Resource*                                     resource,
            const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC> &desc = std::nullopt) const;

        [[nodiscard]] VERTIX_API DescriptorHandle CreateCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) const;

        [[nodiscard]] VERTIX_API DescriptorHandle CreateUAV(
            ID3D12Resource*                                        resource,
            const std::optional<D3D12_UNORDERED_ACCESS_VIEW_DESC> &desc            = std::nullopt,
            ID3D12Resource*                                        counterResource = nullptr) const;

        [[nodiscard]] VERTIX_API DescriptorHandle CreateSRV(
            ID3D12Resource*                                       resource,
            const std::optional<D3D12_SHADER_RESOURCE_VIEW_DESC> &desc = std::nullopt) const;

        void SetDescriptorHeaps(ID3D12GraphicsCommandList* cmdList) const { cmdList->SetDescriptorHeaps(2, gpuHeaps); }

        [[nodiscard]] DescriptorHeap* operator[](const D3D12_DESCRIPTOR_HEAP_TYPE heapType) const noexcept { return descriptorHeaps[heapType].get(); }
    };
}
