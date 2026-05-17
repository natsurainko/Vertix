//
// Created by Natsurainko on 2026/4/30.
//

#ifndef VERTIX_RENDERTEXTURE_H
#define VERTIX_RENDERTEXTURE_H

#include <optional>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "RenderResource.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    class RenderTexture : public RenderResource {
    public:
        RenderTexture(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_DESC &resourceDesc,
            const D3D12_RESOURCE_STATES resourceState,
            const std::optional<D3D12_CLEAR_VALUE> &clearValue = std::nullopt)
        : RenderResource(RenderResourceKind::Texture, d3d12Resource, resourceState), resourceDesc(resourceDesc), clearValue(clearValue) {}

        virtual void Resize(
            ID3D12Device* d3d12Device,
            const uint64_t size[3]) = 0;

    protected:
        D3D12_RESOURCE_DESC resourceDesc;
        std::optional<D3D12_CLEAR_VALUE> clearValue = std::nullopt;
    };

    class RenderTexture1D : public RenderTexture {
    public:
        RenderTexture1D(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_DESC &resourceDesc,
            const D3D12_RESOURCE_STATES resourceState,
            const std::optional<D3D12_CLEAR_VALUE> &clearValue = std::nullopt)
        : RenderTexture(d3d12Resource, resourceDesc, resourceState, clearValue) {}

        VERTIX_API void Resize(
            ID3D12Device* d3d12Device,
            const uint64_t size[3]) override;
    };
    class RenderTexture2D : public RenderTexture {
    public:
        RenderTexture2D(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_DESC &resourceDesc,
            const D3D12_RESOURCE_STATES resourceState,
            const std::optional<D3D12_CLEAR_VALUE> &clearValue = std::nullopt)
        : RenderTexture(d3d12Resource, resourceDesc, resourceState, clearValue) {}

        VERTIX_API void Resize(
            ID3D12Device* d3d12Device,
            const uint64_t size[3]) override;

        VERTIX_API void Resize(
            ID3D12Device* d3d12Device,
            Vector2D<UINT> size);
    };
    class RenderTexture3D : public RenderTexture {
    public:
        RenderTexture3D(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_DESC &resourceDesc,
            const D3D12_RESOURCE_STATES resourceState,
            const std::optional<D3D12_CLEAR_VALUE> &clearValue = std::nullopt)
        : RenderTexture(d3d12Resource, resourceDesc, resourceState, clearValue) {}

        VERTIX_API void Resize(
            ID3D12Device* d3d12Device,
            const uint64_t size[3]) override;
    };
}

#endif //VERTIX_RENDERTEXTURE_H
