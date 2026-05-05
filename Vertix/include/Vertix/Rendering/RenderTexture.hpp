//
// Created by Natsurainko on 2026/4/30.
//

#ifndef VERTIX_RENDERTEXTURE_H
#define VERTIX_RENDERTEXTURE_H

#include <memory>
#include <optional>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "RenderTextureAccessor.h"
#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    template<RenderTextureAccessor Accessor>
    class RenderTexture {
        static_assert(
            !(Accessor & RenderTarget && Accessor & DepthStencil),
            "RenderTarget and DepthStencil cannot be combined on the same RenderTexture."
        );
    public:
        struct ScopedTransition {
            RenderTexture*             texture;
            ID3D12GraphicsCommandList* cmd;
            D3D12_RESOURCE_STATES      restoreState;

            ScopedTransition(RenderTexture* texture, ID3D12GraphicsCommandList* cmd, const D3D12_RESOURCE_STATES restoreState) noexcept
                : texture(texture), cmd(cmd), restoreState(restoreState) {}

            ScopedTransition(const ScopedTransition&)            = delete;
            ScopedTransition& operator=(const ScopedTransition&) = delete;

            ScopedTransition(ScopedTransition&& other) noexcept : texture(other.texture), cmd(other.cmd), restoreState(other.restoreState) {
                other.texture = nullptr;
            }

            ~ScopedTransition() { texture->Transition(cmd, restoreState); }
        };

        RenderTexture(
            const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON)
        : d3d12Device(d3d12Device), d3d12Resource(d3d12Resource), currentResourceState(resourceState), resourceDesc(d3d12Resource->GetDesc()) {}

        RenderTexture(
            const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
            const D3D12_RESOURCE_DESC* resourceDesc,
            const D3D12_CLEAR_VALUE* clearValue = nullptr)
        : d3d12Device(d3d12Device), currentResourceState(D3D12_RESOURCE_STATE_COMMON), resourceDesc(*resourceDesc)
        {
            this->clearValue = clearValue ? std::make_optional(*clearValue) : std::nullopt;
            const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

            if constexpr (Accessor & RenderTarget) {
                this->resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            }
            if constexpr (Accessor & DepthStencil) {
                this->resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
                currentResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            }
            if constexpr (Accessor & UnorderedAccess) {
                this->resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

                if (currentResourceState == D3D12_RESOURCE_STATE_COMMON)
                    currentResourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            }

            ThrowIfFailed(d3d12Device->CreateCommittedResource(
                &defaultHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &this->resourceDesc,
                currentResourceState,
                clearValue,
                IID_PPV_ARGS(&d3d12Resource)
            ));
        }

        void Transition(ID3D12GraphicsCommandList* cmd, const D3D12_RESOURCE_STATES newState) {
            if (currentResourceState == newState) return;

            const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), currentResourceState, newState);
            cmd->ResourceBarrier(1, &barrier);
            currentResourceState = newState;
        }

        [[nodiscard]]
        ScopedTransition ScopedTransition(ID3D12GraphicsCommandList* cmd, const D3D12_RESOURCE_STATES targetState) {
            const D3D12_RESOURCE_STATES prev = currentResourceState;
            Transition(cmd, targetState);
            return { this, cmd, prev };
        }

        void Resize(const Vector2D<UINT> size) {
            resourceDesc.Width = (std::max)(size.X, static_cast<UINT>(1));
            resourceDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

            d3d12Resource.Reset();

            const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
            ThrowIfFailed(d3d12Device->CreateCommittedResource(
                &defaultHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                currentResourceState,
                clearValue.has_value() ? &clearValue.value() : nullptr,
                IID_PPV_ARGS(&d3d12Resource)
            ));
        }

        void Reset() {
            d3d12Resource.Reset();
        }

        void Replace(const Microsoft::WRL::ComPtr<ID3D12Resource> &newResource) {
            d3d12Resource.Reset();
            d3d12Resource = newResource;
            resourceDesc = d3d12Resource->GetDesc();
        }

        [[nodiscard]] ID3D12Resource*       GetResource()     const { return d3d12Resource.Get(); }
        [[nodiscard]] D3D12_RESOURCE_STATES GetCurrentState() const { return currentResourceState; }

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;

        D3D12_RESOURCE_STATES currentResourceState;
        D3D12_RESOURCE_DESC resourceDesc;
        std::optional<D3D12_CLEAR_VALUE> clearValue = std::nullopt;
    };
}

#endif //VERTIX_RENDERTEXTURE_H
