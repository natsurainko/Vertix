//
// Created by Natsurainko on 2026/5/19.
//

#pragma once

#include <variant>

#include "DescriptorHandle.h"
#include "Vertix/Rendering/RenderResourceUsage.h"

namespace Vertix {
    using DescriptorViewDesc = std::variant<
        std::monostate,
        D3D12_RENDER_TARGET_VIEW_DESC,
        D3D12_DEPTH_STENCIL_VIEW_DESC,
        D3D12_UNORDERED_ACCESS_VIEW_DESC,
        D3D12_SHADER_RESOURCE_VIEW_DESC,
        D3D12_CONSTANT_BUFFER_VIEW_DESC
    >;

    inline bool operator==(const DescriptorViewDesc &a, const DescriptorViewDesc &b) {
        if (a.index() != b.index()) return false;

        return std::visit(
            [&]<typename T>(const T &lhs) -> bool {
                const T &rhs = std::get<T>(b);
                if constexpr (std::is_same_v<T, std::monostate>) {
                    return true;
                } else {
                    return std::memcmp(&lhs, &rhs, sizeof(T)) == 0;
                }
            },
            a
        );
    }

    template <RenderResourceUsage>
    struct DescriptorView : DescriptorHandle {
        using DescriptorHandle::operator=;
    };

    template <>
    struct DescriptorView<RenderResourceUsage::DepthRead> : DescriptorHandle {
        using DescriptorHandle::operator=;
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd) const { cmd->OMSetRenderTargets(0, nullptr, FALSE, &cpuHandle); }
    };

    template <>
    struct DescriptorView<RenderResourceUsage::DepthWrite> : DescriptorHandle {
        using DescriptorHandle::operator=;
        void ClearDepth(ID3D12GraphicsCommandList* cmd, const float depth = 1.0f) const { cmd->ClearDepthStencilView(cpuHandle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr); }
        void ClearStencil(ID3D12GraphicsCommandList* cmd, const UINT8 stencil = 0) const { cmd->ClearDepthStencilView(cpuHandle, D3D12_CLEAR_FLAG_STENCIL, 0.0f, stencil, 0, nullptr); }
        void ClearDepthStencil(ID3D12GraphicsCommandList* cmd, const float depth = 1.0f, const UINT8 stencil = 0) const { cmd->ClearDepthStencilView(cpuHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd) const { cmd->OMSetRenderTargets(0, nullptr, FALSE, &cpuHandle); }
    };

    template <>
    struct DescriptorView<RenderResourceUsage::RenderTarget> : DescriptorHandle {
        using DescriptorHandle::operator=;
        void Clear(ID3D12GraphicsCommandList* cmd, const float clearColor[4]) const { cmd->ClearRenderTargetView(cpuHandle, clearColor, 0, nullptr); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr) const { cmd->OMSetRenderTargets(1, &cpuHandle, FALSE, dsvHandle); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd, const DescriptorHandle* dsv) const { cmd->OMSetRenderTargets(1, &cpuHandle, FALSE, &dsv->cpuHandle); }
    };

    template <>
    struct DescriptorView<RenderResourceUsage::UnorderedAccess> : DescriptorHandle {
        using DescriptorHandle::operator=;
        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle); }
        void SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetComputeRootDescriptorTable(rootParameterIndex, gpuHandle); }
    };

    template <>
    struct DescriptorView<RenderResourceUsage::PixelShaderResource> : DescriptorHandle {
        using DescriptorHandle::operator=;
        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle); }
    };

    template <>
    struct DescriptorView<RenderResourceUsage::NonPixelShaderResource> : DescriptorHandle {
        using DescriptorHandle::operator=;
        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle); }
        void SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetComputeRootDescriptorTable(rootParameterIndex, gpuHandle); }
    };

    template <>
    struct DescriptorView<RenderResourceUsage::AllShaderResource> : DescriptorHandle {
        using DescriptorHandle::operator=;
        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle); }
        void SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetComputeRootDescriptorTable(rootParameterIndex, gpuHandle); }
    };
}
