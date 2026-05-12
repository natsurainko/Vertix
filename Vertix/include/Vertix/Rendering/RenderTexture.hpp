//
// Created by Natsurainko on 2026/4/30.
//

#ifndef VERTIX_RENDERTEXTURE_H
#define VERTIX_RENDERTEXTURE_H

#include <memory>
#include <optional>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "RenderResource.h"
#include "RenderResourceAccessor.h"
#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    template<RenderResourceAccessor Accessor>
    class RenderTexture;

    class RenderTextureBase : public RenderResource {
    public:
        RenderTextureBase(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES currentResourceState)
        : RenderResource(d3d12Resource, currentResourceState) {}

        virtual void Resize(
            ID3D12Device* d3d12Device,
            Vector2D<UINT> size) = 0;

        [[nodiscard]]
        virtual RenderResourceAccessor GetAccessor() const = 0;

        template <RenderResourceAccessor Accessor>
        static std::unique_ptr<RenderTexture<Accessor>> Create(
            const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
            const D3D12_RESOURCE_DESC &desc,
            const std::optional<D3D12_CLEAR_VALUE> &clearValue)
        {
            static_assert(
                !(Accessor & RenderTarget && Accessor & DepthStencil),
                "RenderTarget and DepthStencil cannot be combined on the same RenderTexture."
            );

            D3D12_RESOURCE_DESC   resourceDesc  = desc;
            D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON;
            std::optional<D3D12_CLEAR_VALUE> clearValueOptional = clearValue;

            std::optional<D3D12_RESOURCE_STATES> resourceStateOptional = std::nullopt;

            if constexpr (Accessor & RenderTarget) {
                resourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
                resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                resourceStateOptional = std::optional(resourceState);
            }
            if constexpr (Accessor & DepthStencil) {
                resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
                resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
                resourceStateOptional = std::optional(resourceState);
            }
            if constexpr (Accessor & UnorderedAccess) {
                resourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
                resourceStateOptional = std::optional(resourceState);
            }

            if (clearValueOptional.has_value() && clearValueOptional.value().Format == DXGI_FORMAT_UNKNOWN) {
                clearValueOptional.value().Format = resourceDesc.Format;
            }

            Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
            const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
            ThrowIfFailed(d3d12Device->CreateCommittedResource(
                &defaultHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                resourceState,
                (resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) && clearValueOptional.has_value() ? &clearValueOptional.value() : nullptr,
                IID_PPV_ARGS(&d3d12Resource)
            ));

            return std::make_unique<RenderTexture<Accessor>>(d3d12Resource, resourceDesc, clearValueOptional, resourceStateOptional);
        }

        template <RenderResourceAccessor Accessor>
        static std::unique_ptr<RenderTexture<Accessor>> Create(
            const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
            const D3D12_RESOURCE_DESC &desc)
        {
            static_assert(
                !(Accessor & RenderTarget && Accessor & DepthStencil),
                "RenderTarget and DepthStencil cannot be combined on the same RenderTexture."
            );

            std::optional<D3D12_CLEAR_VALUE> clearValueOptional = std::nullopt;
            if constexpr (Accessor & RenderTarget) {
                clearValueOptional = std::optional(D3D12_CLEAR_VALUE { .Color = { 0.0f, 0.0f, 0.0f, 0.0f } });
            }
            if constexpr (Accessor & DepthStencil) {
                clearValueOptional = std::optional(D3D12_CLEAR_VALUE { .DepthStencil = { .Depth = 1.0f, .Stencil = 0 } });
            }

            return Create<Accessor>(d3d12Device, desc, clearValueOptional);
        }
    };

    template<RenderResourceAccessor Accessor>
    class RenderTexture : public RenderTextureBase {
        static_assert(
            !(Accessor & RenderTarget && Accessor & DepthStencil),
            "RenderTarget and DepthStencil cannot be combined on the same RenderTexture."
        );
    public:
        explicit RenderTexture(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_DESC &resourceDesc,
            const std::optional<D3D12_CLEAR_VALUE> &clearValue = std::nullopt,
            const std::optional<D3D12_RESOURCE_STATES> resourceState = std::nullopt)
        : RenderTextureBase(d3d12Resource, D3D12_RESOURCE_STATE_COMMON), resourceDesc(resourceDesc), clearValue(clearValue)
        {
            if (resourceState.has_value()) {
                currentResourceState = resourceState.value();
                return;
            }

            if constexpr (Accessor & RenderTarget) {
                currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            }
            if constexpr (Accessor & DepthStencil) {
                currentResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            }
            if constexpr (Accessor & UnorderedAccess) {
                currentResourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            }
        }

        void Resize(
            ID3D12Device* d3d12Device,
            const Vector2D<UINT> size) override
        {
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

        [[nodiscard]] RenderResourceAccessor GetAccessor() const override { return Accessor; }

    private:
        D3D12_RESOURCE_DESC resourceDesc;
        std::optional<D3D12_CLEAR_VALUE> clearValue = std::nullopt;
    };
}

#endif //VERTIX_RENDERTEXTURE_H
