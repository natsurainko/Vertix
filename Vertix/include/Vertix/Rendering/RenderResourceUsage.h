//
// Created by Natsurainko on 2026/5/19.
//

#pragma once

#include <cassert>
#include <cstdint>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_core.h>

namespace Vertix {
    enum class RenderResourceUsage : uint32_t {
        None = 0,

        PixelShaderResource = 1 << 0,    // → PIXEL_SHADER_RESOURCE
        NonPixelShaderResource = 1 << 1, // → NON_PIXEL_SHADER_RESOURCE (compute/vertex)
        ConstantBufferRead = 1 << 2,     // → VERTEX_AND_CONSTANT_BUFFER (CBV)
        VertexBufferRead = 1 << 3,       // → VERTEX_AND_CONSTANT_BUFFER (VB)
        IndexBufferRead = 1 << 4,        // → INDEX_BUFFER
        IndirectArgumentRead = 1 << 5,   // → INDIRECT_ARGUMENT
        CopySource = 1 << 6,             // → COPY_SOURCE
        ResolveSource = 1 << 7,          // → RESOLVE_SOURCE
        DepthRead = 1 << 8,              // → DEPTH_READ

        RenderTarget = 1 << 9,     // → RENDER_TARGET
        DepthWrite = 1 << 10,      // → DEPTH_WRITE
        UnorderedAccess = 1 << 11, // → UNORDERED_ACCESS
        CopyDest = 1 << 12,        // → COPY_DEST
        ResolveDest = 1 << 13,     // → RESOLVE_DEST
        Present = 1 << 14,         // → PRESENT
        StreamOut = 1 << 15,       // → STREAM_OUT

        AccelerationStructure = 1 << 16, // → RAYTRACING_ACCELERATION_STRUCTURE
        ShadingRateSource = 1 << 17,     // → SHADING_RATE_SOURCE

        UploadHeap = 1 << 18,
        ReadbackHeap = 1 << 19,

        AllShaderResource = PixelShaderResource | NonPixelShaderResource, // → ALL_SHADER_RESOURCE
        VertexConstantRead = VertexBufferRead | ConstantBufferRead,       // → VERTEX_AND_CONSTANT_BUFFER
        GenericRead = AllShaderResource | VertexBufferRead | ConstantBufferRead
        | IndexBufferRead | IndirectArgumentRead | CopySource,

        ConstantBuffer = GenericRead | UploadHeap,
        StructuredBuffer = GenericRead,
        RWStructuredBuffer = UnorderedAccess,
    };

    constexpr RenderResourceUsage operator &(RenderResourceUsage a, RenderResourceUsage b) {
        return static_cast<RenderResourceUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr RenderResourceUsage operator |(RenderResourceUsage a, RenderResourceUsage b) {
        return static_cast<RenderResourceUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline D3D12_RESOURCE_STATES DeriveState(const RenderResourceUsage usage) {
        if ((usage & RenderResourceUsage::UploadHeap) != RenderResourceUsage::None) {
            constexpr RenderResourceUsage kGpuWriteFlags =
                    RenderResourceUsage::UnorderedAccess |
                    RenderResourceUsage::RenderTarget |
                    RenderResourceUsage::DepthWrite |
                    RenderResourceUsage::AccelerationStructure;

            assert((usage & kGpuWriteFlags) == RenderResourceUsage::None && "GPU-write usage flags are incompatible with UploadHeap");
            return D3D12_RESOURCE_STATE_GENERIC_READ;
        }
        if ((usage & RenderResourceUsage::ReadbackHeap) != RenderResourceUsage::None) {
            assert(usage == RenderResourceUsage::ReadbackHeap && "ReadbackHeap are incompatible with other flags");
            return D3D12_RESOURCE_STATE_COPY_DEST;
        }
        if ((usage & RenderResourceUsage::AccelerationStructure) != RenderResourceUsage::None) {
            assert(usage == RenderResourceUsage::AccelerationStructure && "AccelerationStructure are incompatible with other flags");
            return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        }

        D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

        if ((usage & RenderResourceUsage::PixelShaderResource) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        if ((usage & RenderResourceUsage::NonPixelShaderResource) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        if ((usage & RenderResourceUsage::ConstantBufferRead) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        if ((usage & RenderResourceUsage::VertexBufferRead) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        if ((usage & RenderResourceUsage::IndexBufferRead) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
        if ((usage & RenderResourceUsage::IndirectArgumentRead) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
        if ((usage & RenderResourceUsage::CopySource) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_COPY_SOURCE;
        if ((usage & RenderResourceUsage::ResolveSource) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        if ((usage & RenderResourceUsage::DepthRead) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_DEPTH_READ;

        if ((usage & RenderResourceUsage::RenderTarget) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_RENDER_TARGET;
        if ((usage & RenderResourceUsage::DepthWrite) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
        if ((usage & RenderResourceUsage::UnorderedAccess) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        if ((usage & RenderResourceUsage::CopyDest) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_COPY_DEST;
        if ((usage & RenderResourceUsage::ResolveDest) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
        if ((usage & RenderResourceUsage::Present) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_PRESENT;
        if ((usage & RenderResourceUsage::StreamOut) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_STREAM_OUT;

        if ((usage & RenderResourceUsage::ShadingRateSource) != RenderResourceUsage::None)
            state |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;

        return state;
    }

    inline D3D12_HEAP_PROPERTIES DeriveHeapProperties(const RenderResourceUsage usages) {
        if ((usages & RenderResourceUsage::UploadHeap) != RenderResourceUsage::None)
            return CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        if ((usages & RenderResourceUsage::ReadbackHeap) != RenderResourceUsage::None)
            return CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);

        return CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    }

    inline D3D12_RESOURCE_FLAGS DeriveResourceFlags(const RenderResourceUsage usages) {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
        if ((usages & RenderResourceUsage::RenderTarget) != RenderResourceUsage::None)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        if ((usages & RenderResourceUsage::DepthRead) != RenderResourceUsage::None)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        if ((usages & RenderResourceUsage::DepthWrite) != RenderResourceUsage::None)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        if ((usages & RenderResourceUsage::UnorderedAccess) != RenderResourceUsage::None)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        /*if ((usages & RenderResourceUsage::AllShaderResource) == RenderResourceUsage::None)
            flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;*/

        if ((usages & RenderResourceUsage::AccelerationStructure) != RenderResourceUsage::None)
            flags |= D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;

        return flags;
    }

    namespace detail {
        inline constexpr RenderResourceUsage kReadUsageMask =
                RenderResourceUsage::PixelShaderResource |
                RenderResourceUsage::NonPixelShaderResource |
                RenderResourceUsage::ConstantBufferRead |
                RenderResourceUsage::VertexBufferRead |
                RenderResourceUsage::IndexBufferRead |
                RenderResourceUsage::IndirectArgumentRead |
                RenderResourceUsage::CopySource |
                RenderResourceUsage::ResolveSource |
                RenderResourceUsage::DepthRead |
                RenderResourceUsage::AccelerationStructure |
                RenderResourceUsage::ShadingRateSource |
                RenderResourceUsage::Present |
                RenderResourceUsage::UploadHeap;

        inline constexpr RenderResourceUsage kWriteUsageMask =
                RenderResourceUsage::RenderTarget |
                RenderResourceUsage::DepthWrite |
                RenderResourceUsage::UnorderedAccess |
                RenderResourceUsage::CopyDest |
                RenderResourceUsage::ResolveDest |
                RenderResourceUsage::StreamOut |
                RenderResourceUsage::ReadbackHeap;

        static_assert(
            (kReadUsageMask & kWriteUsageMask) == RenderResourceUsage::None,
            "Read and Write usage masks must not overlap"
        );
    }

    template <RenderResourceUsage Usage>
    concept RenderResourceReadUsage = Usage != RenderResourceUsage::None && (Usage & detail::kReadUsageMask) == Usage;

    template <RenderResourceUsage Usage>
    concept RenderResourceWriteUsage = Usage != RenderResourceUsage::None && (Usage & detail::kWriteUsageMask) == Usage;

    [[nodiscard]] constexpr bool IsReadUsage(const RenderResourceUsage usage) noexcept {
        return usage != RenderResourceUsage::None && (usage & detail::kReadUsageMask) == usage;
    }

    [[nodiscard]] constexpr bool IsWriteUsage(const RenderResourceUsage usage) noexcept {
        return usage != RenderResourceUsage::None && (usage & detail::kWriteUsageMask) == usage;
    }
}
