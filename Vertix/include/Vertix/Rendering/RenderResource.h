//
// Created by Natsurainko on 2026/5/8.
//

#pragma once

#include <d3d12/d3d12.h>
#include <wrl/client.h>

namespace Vertix {
    enum class RenderResourceKind {
        None,
        Texture,
        Buffer,
    };

    class RenderResource {
    protected:
        RenderResourceKind                     kind;
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        D3D12_RESOURCE_STATES                  currentResourceState = D3D12_RESOURCE_STATE_COMMON;

    public:
        RenderResource(
            const RenderResourceKind                      kind,
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES                   currentResourceState)
        : kind(kind),
          d3d12Resource(d3d12Resource),
          currentResourceState(currentResourceState) {}

        virtual ~RenderResource() = default;

        VERTIX_API void Transition(
            ID3D12GraphicsCommandList* cmd,
            D3D12_RESOURCE_STATES      newState) noexcept;

        void Reset() noexcept { d3d12Resource.Reset(); }

        [[nodiscard]] ID3D12Resource*           GetResource() const noexcept { return d3d12Resource.Get(); }
        [[nodiscard]] D3D12_RESOURCE_DESC       GetDesc() const noexcept { return d3d12Resource->GetDesc(); }
        [[nodiscard]] RenderResourceKind        GetResourceKind() const noexcept { return kind; }
        [[nodiscard]] D3D12_RESOURCE_STATES     GetCurrentState() const noexcept { return currentResourceState; }
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const noexcept { return d3d12Resource->GetGPUVirtualAddress(); }
    };
}
