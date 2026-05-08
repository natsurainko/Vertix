//
// Created by Natsurainko on 2026/5/5.
//

#ifndef VERTIX_RENDERTEXTUREVIEWS_H
#define VERTIX_RENDERTEXTUREVIEWS_H

#include <d3d12/d3d12.h>
#include <optional>

#include "Vertix/Graphics/DescriptorHeap.h"
#include "Vertix/Rendering/RenderResourceAccessor.h"

namespace Vertix {
    namespace detail {
        struct RenderResourceViewBase_Cpu {
            RenderResourceViewBase_Cpu() = default;
            RenderResourceViewBase_Cpu(const RenderResourceViewBase_Cpu&)            = delete;
            RenderResourceViewBase_Cpu& operator=(const RenderResourceViewBase_Cpu&) = delete;

            RenderResourceViewBase_Cpu(RenderResourceViewBase_Cpu&& o) noexcept
                : handle(o.handle), heap(o.heap) { o.heap = nullptr; }

            RenderResourceViewBase_Cpu& operator=(RenderResourceViewBase_Cpu&& o) noexcept {
                if (this != &o) {
                    if (heap && handle.ptr) heap->FreeDescriptorHandle(handle);
                    handle = o.handle;
                    heap   = o.heap;
                    o.heap = nullptr;
                }
                return *this;
            }

            ~RenderResourceViewBase_Cpu() { if (heap && handle.ptr) heap->FreeDescriptorHandle(handle); }

            [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetHandle() const { return handle; }
            [[nodiscard]] const D3D12_CPU_DESCRIPTOR_HANDLE* GetHandleAddress() const { return &handle; }
            [[nodiscard]] DescriptorHeap* GetHeap() const { return heap; }
        protected:
            D3D12_CPU_DESCRIPTOR_HANDLE handle{};
            DescriptorHeap* heap = nullptr;
        };
        struct RenderResourceViewBase_CpuGpu {
            RenderResourceViewBase_CpuGpu() = default;
            RenderResourceViewBase_CpuGpu(const RenderResourceViewBase_CpuGpu&)            = delete;
            RenderResourceViewBase_CpuGpu& operator=(const RenderResourceViewBase_CpuGpu&) = delete;

            RenderResourceViewBase_CpuGpu(RenderResourceViewBase_CpuGpu&& o) noexcept
                : cpuHandle(o.cpuHandle), gpuHandle(o.gpuHandle), heap(o.heap) { o.heap = nullptr; }

            RenderResourceViewBase_CpuGpu& operator=(RenderResourceViewBase_CpuGpu&& o) noexcept {
                if (this != &o) {
                    if (heap && cpuHandle.ptr) heap->FreeDescriptorHandle(cpuHandle);
                    cpuHandle = o.cpuHandle;
                    gpuHandle = o.gpuHandle;
                    heap      = o.heap;
                    o.heap    = nullptr;
                }
                return *this;
            }

            ~RenderResourceViewBase_CpuGpu() { if (heap && cpuHandle.ptr) heap->FreeDescriptorHandle(cpuHandle); }

            [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return cpuHandle; }
            [[nodiscard]] const D3D12_CPU_DESCRIPTOR_HANDLE* GetCpuHandleAddress() const { return &cpuHandle; }
            [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return gpuHandle; }
            [[nodiscard]] const D3D12_GPU_DESCRIPTOR_HANDLE* GetGpuHandleAddress() const { return &gpuHandle; }
            [[nodiscard]] DescriptorHeap* GetHeap() const { return heap; }
        protected:
            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
            D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
            DescriptorHeap* heap = nullptr;
        };
    }

    template<RenderResourceAccessor A>
    concept SingleAccessor = A == RenderTarget ||
                             A == DepthStencil ||
                             A == UnorderedAccess ||
                             A == ShaderResource;

    template<RenderResourceAccessor Accessor> requires SingleAccessor<Accessor>
    struct RenderResourceView {};

    template<>
    struct RenderResourceView<DepthStencil> : detail::RenderResourceViewBase_Cpu {
        friend class RenderResourceViewAllocator;
        void ClearDepth(ID3D12GraphicsCommandList* cmd, const float depth = 1.0f) const { cmd->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr); }
        void ClearStencil(ID3D12GraphicsCommandList* cmd, const UINT8 stencil = 0) const { cmd->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_STENCIL, 0.0f, stencil, 0, nullptr); }
        void ClearDepthStencil(ID3D12GraphicsCommandList* cmd, const float depth = 1.0f, const UINT8 stencil = 0) const { cmd->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd) const { cmd->OMSetRenderTargets(0, nullptr, FALSE, &handle); }

        void Reuse(ID3D12Device* d3d12Device, ID3D12Resource* d3d12Resource) const {
            d3d12Device->CreateDepthStencilView(d3d12Resource, cachedDesc.has_value() ? &cachedDesc.value() : nullptr, handle);
        }
    private:
        std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC> cachedDesc = std::nullopt;
    };

    template<>
    struct RenderResourceView<RenderTarget> : detail::RenderResourceViewBase_Cpu {
        friend class RenderResourceViewAllocator;
        void Clear(ID3D12GraphicsCommandList* cmd, const float clearColor[4]) const { cmd->ClearRenderTargetView(handle, clearColor, 0, nullptr); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle = nullptr) const { cmd->OMSetRenderTargets(1, &handle, FALSE, dsvHandle); }
        void SetRenderTarget(ID3D12GraphicsCommandList* cmd, const RenderResourceView<DepthStencil>* dsv) const { cmd->OMSetRenderTargets(1, &handle, FALSE, dsv->GetHandleAddress()); }

        void Reuse(ID3D12Device* d3d12Device, ID3D12Resource* d3d12Resource) const {
            d3d12Device->CreateRenderTargetView(d3d12Resource, cachedDesc.has_value() ? &cachedDesc.value() : nullptr, handle);
        }
    private:
        std::optional<D3D12_RENDER_TARGET_VIEW_DESC> cachedDesc = std::nullopt;
    };

    template<>
    struct RenderResourceView<UnorderedAccess> : detail::RenderResourceViewBase_CpuGpu {
        friend class RenderResourceViewAllocator;
        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle); }
        void SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetComputeRootDescriptorTable(rootParameterIndex, gpuHandle); }

        void Reuse(ID3D12Device* d3d12Device, ID3D12Resource* d3d12Resource) const {
            d3d12Device->CreateUnorderedAccessView(d3d12Resource, cachedCounterResource, cachedDesc.has_value() ? &cachedDesc.value() : nullptr, cpuHandle);
        }
        void Reuse(ID3D12Device* d3d12Device, ID3D12Resource* d3d12Resource, ID3D12Resource* counterResource) {
            cachedCounterResource = counterResource;
            d3d12Device->CreateUnorderedAccessView(d3d12Resource, counterResource, cachedDesc.has_value() ? &cachedDesc.value() : nullptr, cpuHandle);
        }
    private:
        ID3D12Resource* cachedCounterResource = nullptr;
        std::optional<D3D12_UNORDERED_ACCESS_VIEW_DESC> cachedDesc = std::nullopt;
    };

    template<>
    struct RenderResourceView<ShaderResource> : detail::RenderResourceViewBase_CpuGpu {
        friend class RenderResourceViewAllocator;
        void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle); }
        void SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* cmd, const UINT rootParameterIndex) const { cmd->SetComputeRootDescriptorTable(rootParameterIndex, gpuHandle); }

        void Reuse(ID3D12Device* d3d12Device, ID3D12Resource* d3d12Resource) const {
            d3d12Device->CreateShaderResourceView(d3d12Resource, cachedDesc.has_value() ? &cachedDesc.value() : nullptr, cpuHandle);
        }
    private:
        std::optional<D3D12_SHADER_RESOURCE_VIEW_DESC> cachedDesc = std::nullopt;
    };
}

#endif //VERTIX_RENDERTEXTUREVIEWS_H
