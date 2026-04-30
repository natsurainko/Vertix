//
// Created by Natsurainko on 2026/4/30.
//

#ifndef VERTIX_RENDERTEXTURE_H
#define VERTIX_RENDERTEXTURE_H

#include <cassert>
#include <memory>
#include <optional>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "RenderTextureAllocator.hpp"
#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    enum RenderTextureAccessor {
        RenderTarget    = 0x1,
        DepthStencil    = 0x2,
        UnorderedAccess = 0x4,
        ShaderResource  = 0x8,

        DrawColorSampleAccessor         = RenderTarget    | ShaderResource,
        DrawDepthSampleAccessor         = DepthStencil    | ShaderResource,
        UnorderedAccessSampleAccessor   = UnorderedAccess | ShaderResource,
    };

    struct RenderTextureDepthStencilView {
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};

        void ClearDepth(ID3D12GraphicsCommandList* cmd, const float depth = 1.0f) const {
            cmd->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
        }

        void ClearStencil(ID3D12GraphicsCommandList* cmd, const UINT8 stencil = 0) const {
            cmd->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_STENCIL, 0.0f, stencil, 0, nullptr);
        }

        void ClearDepthStencil(ID3D12GraphicsCommandList* cmd, const float depth = 1.0f, const UINT8 stencil = 0) const {
            cmd->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
        }
    };

    struct RenderTextureRenderTargetView {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};

        void Clear(ID3D12GraphicsCommandList* cmd, const float clearColor[4]) const {
            cmd->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        }

        void SetRenderTarget(ID3D12GraphicsCommandList* cmd, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr) const {
            cmd->OMSetRenderTargets(1, &rtvHandle, FALSE, dsvHandle);
        }

        void SetRenderTarget(ID3D12GraphicsCommandList* cmd, const RenderTextureDepthStencilView* dsv) const {
            cmd->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsv->dsvHandle);
        }
    };

    struct RenderTextureUnorderedAccessView {
        D3D12_CPU_DESCRIPTOR_HANDLE uavCpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE uavGpuHandle{};
    };

    struct RenderTextureShaderResourceView {
        D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle{};

        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const {
            cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, srvGpuHandle);
        }

        void SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const {
            cmd->SetComputeRootDescriptorTable(rootParameterIndex, srvGpuHandle);
        }
    };

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
            RenderTextureAllocator* allocator,
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON) : allocator(allocator), d3d12Resource(d3d12Resource), currentResourceState(resourceState)
        {
            assert(allocator != nullptr);
            d3d12Device = allocator->GetGraphicsDevice()->GetD3D12Device();
            resourceDesc = d3d12Resource->GetDesc();

            if constexpr (Accessor & RenderTarget) {
                allocator->GetRenderTargetDescriptorHeap()->AllocDescriptorHandle(rtvHandle);
            }
            if constexpr (Accessor & DepthStencil) {
                allocator->GetDepthStencilDescriptorHeap()->AllocDescriptorHandle(dsvHandle);
            }
            if constexpr (Accessor & UnorderedAccess) {
                allocator->GetUnorderedAccessDescriptorHeap()->AllocDescriptorHandle(uavCpuHandle, uavGpuHandle);
            }
            if constexpr (Accessor & ShaderResource) {
                allocator->GetShaderResourceDescriptorHeap()->AllocDescriptorHandle(srvCpuHandle, srvGpuHandle);
            }
        }

        RenderTexture(
            RenderTextureAllocator* allocator,
            const D3D12_RESOURCE_DESC* resourceDesc,
            const D3D12_CLEAR_VALUE* clearValue = nullptr) : allocator(allocator), ownsResource(true), resourceDesc(*resourceDesc)
        {
            assert(allocator != nullptr);
            d3d12Device = allocator->GetGraphicsDevice()->GetD3D12Device();
            this->clearValue = clearValue ? std::make_optional(*clearValue) : std::nullopt;

            if constexpr (Accessor & RenderTarget) {
                this->resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
                allocator->GetRenderTargetDescriptorHeap()->AllocDescriptorHandle(rtvHandle);
            }
            if constexpr (Accessor & DepthStencil) {
                this->resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
                currentResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
                allocator->GetDepthStencilDescriptorHeap()->AllocDescriptorHandle(dsvHandle);
            }
            if constexpr (Accessor & UnorderedAccess) {
                this->resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

                if (currentResourceState == D3D12_RESOURCE_STATE_COMMON)
                    currentResourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                allocator->GetUnorderedAccessDescriptorHeap()->AllocDescriptorHandle(uavCpuHandle, uavGpuHandle);
            }
            if constexpr (Accessor & ShaderResource) {
                allocator->GetShaderResourceDescriptorHeap()->AllocDescriptorHandle(srvCpuHandle, srvGpuHandle);
            }

            const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
            ThrowIfFailed(d3d12Device->CreateCommittedResource(
                &defaultHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &this->resourceDesc,
                currentResourceState,
                clearValue,
                IID_PPV_ARGS(&d3d12Resource)
            ));
        }

        [[nodiscard]]
        const RenderTextureRenderTargetView* CreateRenderTargetView(
            const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc = nullptr)
            requires ((Accessor & RenderTarget) != 0)
        {
            d3d12Device->CreateRenderTargetView(d3d12Resource.Get(), rtvDesc, rtvHandle);
            rtv = std::make_unique<RenderTextureRenderTargetView>(rtvHandle);
            cachedRtvDesc = rtvDesc ? std::make_optional(*rtvDesc) : std::nullopt;
            return rtv.get();
        }

        [[nodiscard]]
        const RenderTextureDepthStencilView* CreateDepthStencilView(
            const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc = nullptr)
            requires ((Accessor & DepthStencil) != 0)
        {
            d3d12Device->CreateDepthStencilView(d3d12Resource.Get(), dsvDesc, dsvHandle);
            dsv = std::make_unique<RenderTextureDepthStencilView>(dsvHandle);
            cachedDsvDesc = dsvDesc ? std::make_optional(*dsvDesc) : std::nullopt;
            return dsv.get();
        }

        [[nodiscard]]
        const RenderTextureUnorderedAccessView* CreateUnorderedAccessView(
            const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr,
            ID3D12Resource* pCounterResource = nullptr)
            requires ((Accessor & UnorderedAccess) != 0)
        {
            d3d12Device->CreateUnorderedAccessView(d3d12Resource.Get(), pCounterResource, uavDesc, uavCpuHandle);
            uav = std::make_unique<RenderTextureUnorderedAccessView>(uavCpuHandle, uavGpuHandle);
            cachedCounterResource = pCounterResource;
            cachedUavDesc = uavDesc ? std::make_optional(*uavDesc) : std::nullopt;
            return uav.get();
        }

        [[nodiscard]]
        const RenderTextureShaderResourceView* CreateShaderResourceView(
            const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr)
            requires ((Accessor & ShaderResource) != 0)
        {
            d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), srvDesc, srvCpuHandle);
            srv = std::make_unique<RenderTextureShaderResourceView>(srvCpuHandle, srvGpuHandle);
            cachedSrvDesc = srvDesc ? std::make_optional(*srvDesc) : std::nullopt;
            return srv.get();
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
            assert(ownsResource && "Cannot resize a RenderTexture wrapping an external resource.");

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

            if constexpr (Accessor & RenderTarget) {
                if (rtv) {
                    d3d12Device->CreateRenderTargetView(d3d12Resource.Get(), cachedRtvDesc.has_value() ? &cachedRtvDesc.value() : nullptr, rtvHandle);
                }
            }
            if constexpr (Accessor & DepthStencil) {
                if (dsv) {
                    d3d12Device->CreateDepthStencilView(d3d12Resource.Get(), cachedDsvDesc.has_value() ? &cachedDsvDesc.value() : nullptr, dsvHandle);
                }
            }
            if constexpr (Accessor & UnorderedAccess) {
                if (uav) {
                    d3d12Device->CreateUnorderedAccessView(d3d12Resource.Get(), cachedCounterResource, cachedUavDesc.has_value() ? &cachedUavDesc.value() : nullptr, uavCpuHandle);
                }
            }
            if constexpr (Accessor & ShaderResource) {
                if (srv) {
                    d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), cachedSrvDesc.has_value() ? &cachedSrvDesc.value() : nullptr, srvCpuHandle);
                }
            }
        }

        void ResetResource() {
            d3d12Resource.Reset();
        }

        void ReplaceResource(const Microsoft::WRL::ComPtr<ID3D12Resource> &newResource) {
            assert(!ownsResource && "Only external resource can replace by an another resource.");

            this->d3d12Resource = newResource;
            resourceDesc = d3d12Resource->GetDesc();

            if constexpr (Accessor & RenderTarget) {
                if (rtv) {
                    d3d12Device->CreateRenderTargetView(d3d12Resource.Get(), cachedRtvDesc.has_value() ? &cachedRtvDesc.value() : nullptr, rtvHandle);
                }
            }
            if constexpr (Accessor & DepthStencil) {
                if (dsv) {
                    d3d12Device->CreateDepthStencilView(d3d12Resource.Get(), cachedDsvDesc.has_value() ? &cachedDsvDesc.value() : nullptr, dsvHandle);
                }
            }
            if constexpr (Accessor & UnorderedAccess) {
                if (uav) {
                    d3d12Device->CreateUnorderedAccessView(d3d12Resource.Get(), cachedCounterResource, cachedUavDesc.has_value() ? &cachedUavDesc.value() : nullptr, uavCpuHandle);
                }
            }
            if constexpr (Accessor & ShaderResource) {
                if (srv) {
                    d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), cachedSrvDesc.has_value() ? &cachedSrvDesc.value() : nullptr, srvCpuHandle);
                }
            }
        }

        [[nodiscard]] const RenderTextureRenderTargetView*    GetRenderTargetView() const { return rtv.get(); }
        [[nodiscard]] const RenderTextureDepthStencilView*    GetDepthStencilView() const { return dsv.get(); }
        [[nodiscard]] const RenderTextureUnorderedAccessView* GetUnorderedAccessView() const { return uav.get(); }
        [[nodiscard]] const RenderTextureShaderResourceView*  GetShaderResourceView() const { return srv.get(); }

        [[nodiscard]] ID3D12Resource*       GetResource()     const { return d3d12Resource.Get(); }
        [[nodiscard]] D3D12_RESOURCE_STATES GetCurrentState() const { return currentResourceState; }

    private:
        RenderTextureAllocator* allocator = nullptr;
        bool ownsResource = false;

        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;

        D3D12_RESOURCE_STATES currentResourceState = D3D12_RESOURCE_STATE_COMMON;
        D3D12_RESOURCE_DESC resourceDesc;
        std::optional<D3D12_CLEAR_VALUE> clearValue = std::nullopt;

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
        D3D12_CPU_DESCRIPTOR_HANDLE uavCpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE uavGpuHandle{};
        D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle{};

        std::unique_ptr<RenderTextureRenderTargetView>    rtv;
        std::unique_ptr<RenderTextureDepthStencilView>    dsv;
        std::unique_ptr<RenderTextureUnorderedAccessView> uav;
        std::unique_ptr<RenderTextureShaderResourceView>  srv;

        std::optional<D3D12_RENDER_TARGET_VIEW_DESC>    cachedRtvDesc;
        std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC>    cachedDsvDesc;
        std::optional<D3D12_UNORDERED_ACCESS_VIEW_DESC> cachedUavDesc;
        std::optional<D3D12_SHADER_RESOURCE_VIEW_DESC>  cachedSrvDesc;
        ID3D12Resource* cachedCounterResource = nullptr;
    };
}

#endif //VERTIX_RENDERTEXTURE_H
