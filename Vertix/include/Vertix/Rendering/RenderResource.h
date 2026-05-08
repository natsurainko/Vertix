//
// Created by Natsurainko on 2026/5/8.
//

#ifndef VERTIX_RENDERRESOURCE_H
#define VERTIX_RENDERRESOURCE_H

#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_barriers.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"

namespace Vertix {
    class RenderResource {
    public:
        struct ScopedTransition {
            RenderResource*            resource;
            ID3D12GraphicsCommandList* cmd;
            D3D12_RESOURCE_STATES      restoreState;

            VERTIX_API ScopedTransition(RenderResource* resource, ID3D12GraphicsCommandList* cmd, const D3D12_RESOURCE_STATES restoreState) noexcept
                : resource(resource), cmd(cmd), restoreState(restoreState) {}

            ScopedTransition(const ScopedTransition&)            = delete;
            ScopedTransition& operator=(const ScopedTransition&) = delete;

            VERTIX_API ScopedTransition(ScopedTransition&& other) noexcept : resource(other.resource), cmd(other.cmd), restoreState(other.restoreState) {
                other.resource = nullptr;
            }

            VERTIX_API ~ScopedTransition() { resource->Transition(cmd, restoreState); }
        };

        RenderResource(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES currentResourceState)
        : d3d12Resource(d3d12Resource), currentResourceState(currentResourceState) {}

        virtual ~RenderResource() = default;

        VERTIX_API void Transition( ID3D12GraphicsCommandList* cmd, D3D12_RESOURCE_STATES newState) noexcept;
        [[nodiscard]] VERTIX_API ScopedTransition TransitionScoped( ID3D12GraphicsCommandList* cmd, D3D12_RESOURCE_STATES targetState) noexcept;
        [[nodiscard]] VERTIX_API ScopedTransition TransitionScoped( ID3D12GraphicsCommandList* cmd, D3D12_RESOURCE_STATES targetState, D3D12_RESOURCE_STATES restoreState) noexcept;
        [[nodiscard]] VERTIX_API D3D12_RESOURCE_BARRIER CreateTransitionBarrier(const D3D12_RESOURCE_STATES newState) const noexcept { return CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), currentResourceState, newState); }

        void Reset() noexcept { d3d12Resource.Reset(); }

        [[nodiscard]] ID3D12Resource*       GetResource()     const noexcept { return d3d12Resource.Get(); }
        [[nodiscard]] D3D12_RESOURCE_STATES GetCurrentState() const noexcept { return currentResourceState; }
    protected:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        D3D12_RESOURCE_STATES currentResourceState = D3D12_RESOURCE_STATE_COMMON;
    };
}

#endif //VERTIX_RENDERRESOURCE_H
