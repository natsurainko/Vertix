//
// Created by Natsurainko on 2026/5/5.
//

#ifndef VERTIX_RENDERTEXTUREVIEWS_H
#define VERTIX_RENDERTEXTUREVIEWS_H

#include <d3d12/d3d12.h>
#include <variant>
#include <xhash>

#include "RenderResource.h"
#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Graphics/DescriptorHeap.h"

namespace Vertix {
    enum class RenderResourceViewType {
        RenderTarget,
        DepthStencil,
        UnorderedAccess,
        ShaderResource,
        ConstantBuffer,
    };

    struct RenderResourceViewDesc {
        RenderResourceViewType type = {};
        std::variant<
            std::monostate,
            D3D12_RENDER_TARGET_VIEW_DESC,
            D3D12_DEPTH_STENCIL_VIEW_DESC,
            D3D12_UNORDERED_ACCESS_VIEW_DESC,
            D3D12_SHADER_RESOURCE_VIEW_DESC,
            D3D12_CONSTANT_BUFFER_VIEW_DESC
        > desc = std::monostate{};

        VERTIX_API bool operator==(const RenderResourceViewDesc& other) const;
    };

    template<RenderResourceViewType>
    struct RenderResourceView {};

    template<>
    struct RenderResourceView<RenderResourceViewType::DepthStencil> : DescriptorHeapHandle {
        using DescriptorHeapHandle::operator=;
        void ClearDepth(ID3D12GraphicsCommandList* cmd, const float depth = 1.0f) const { cmd->ClearDepthStencilView(cpuHandle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr); }
        void ClearStencil(ID3D12GraphicsCommandList* cmd, const UINT8 stencil = 0) const { cmd->ClearDepthStencilView(cpuHandle, D3D12_CLEAR_FLAG_STENCIL, 0.0f, stencil, 0, nullptr); }
        void ClearDepthStencil(ID3D12GraphicsCommandList* cmd, const float depth = 1.0f, const UINT8 stencil = 0) const { cmd->ClearDepthStencilView(cpuHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd) const { cmd->OMSetRenderTargets(0, nullptr, FALSE, &cpuHandle); }

        void RecreateView(ID3D12Device* device, const RenderResource* renderResource, const RenderResourceViewDesc &viewDesc) const {
            device->CreateDepthStencilView(renderResource->GetResource(),
               viewDesc.desc.index() == 0 ? nullptr : static_cast<const D3D12_DEPTH_STENCIL_VIEW_DESC *>(static_cast<const void*>(&viewDesc.desc)), cpuHandle);
        }
    };

    template<>
    struct RenderResourceView<RenderResourceViewType::RenderTarget> : DescriptorHeapHandle {
        using DescriptorHeapHandle::operator=;
        void Clear(ID3D12GraphicsCommandList* cmd, const float clearColor[4]) const { cmd->ClearRenderTargetView(cpuHandle, clearColor, 0, nullptr); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr) const { cmd->OMSetRenderTargets(1, &cpuHandle, FALSE, dsvHandle); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd, const RenderResourceView<RenderResourceViewType::DepthStencil>* dsv) const { cmd->OMSetRenderTargets(1, &cpuHandle, FALSE, &dsv->cpuHandle); }

        void RecreateView(ID3D12Device* device, const RenderResource* renderResource, const RenderResourceViewDesc &viewDesc) const {
            device->CreateRenderTargetView(renderResource->GetResource(),
                viewDesc.desc.index() == 0 ? nullptr : static_cast<const D3D12_RENDER_TARGET_VIEW_DESC *>(static_cast<const void*>(&viewDesc.desc)), cpuHandle);
        }
    };

    template<>
    struct RenderResourceView<RenderResourceViewType::UnorderedAccess> : DescriptorHeapHandle {
        using DescriptorHeapHandle::operator=;
        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle); }
        void SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetComputeRootDescriptorTable(rootParameterIndex, gpuHandle); }

        void RecreateView(ID3D12Device* device, const RenderResource* renderResource, const RenderResourceViewDesc &viewDesc, const RenderResource* counterResource = nullptr) const {
            device->CreateUnorderedAccessView(renderResource->GetResource(), counterResource ? counterResource->GetResource() : nullptr,
                viewDesc.desc.index() == 0 ? nullptr : static_cast<const D3D12_UNORDERED_ACCESS_VIEW_DESC *>(static_cast<const void*>(&viewDesc.desc)), cpuHandle);
        }
    };

    template<>
    struct RenderResourceView<RenderResourceViewType::ShaderResource> : DescriptorHeapHandle {
        using DescriptorHeapHandle::operator=;
        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle); }
        void SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetComputeRootDescriptorTable(rootParameterIndex, gpuHandle); }

        void RecreateView(ID3D12Device* device, const RenderResource* renderResource, const RenderResourceViewDesc &viewDesc) const {
            device->CreateShaderResourceView(renderResource->GetResource(),
                viewDesc.desc.index() == 0 ? nullptr : static_cast<const D3D12_SHADER_RESOURCE_VIEW_DESC *>(static_cast<const void*>(&viewDesc.desc)), cpuHandle);
        }
    };
}

template <>
struct std::hash<Vertix::RenderResourceViewDesc> {
    VERTIX_API size_t operator()(const Vertix::RenderResourceViewDesc& desc) const noexcept;
};

#endif //VERTIX_RENDERTEXTUREVIEWS_H
