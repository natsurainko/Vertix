//
// Created by Natsurainko on 2026/5/16.
//

#include "Vertix/Rendering/RenderTexture.h"

#include <d3d12/d3dx12_core.h>

#include "Vertix/Exceptions/HResultException.h"

void Vertix::RenderTexture1D::Resize(ID3D12Device *d3d12Device, const uint64_t size[3]) {
    resourceDesc.Width = (std::max)(size[0], static_cast<uint64_t>(1));

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

void Vertix::RenderTexture2D::Resize(ID3D12Device *d3d12Device, const uint64_t size[3]) {
    resourceDesc.Width = (std::max)(static_cast<UINT>(size[0]), static_cast<UINT>(1));
    resourceDesc.Height = (std::max)(static_cast<UINT>(size[1]), static_cast<UINT>(1));

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

void Vertix::RenderTexture2D::Resize(
    ID3D12Device *d3d12Device,
    const Vector2D<UINT> size)
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

void Vertix::RenderTexture3D::Resize(ID3D12Device *d3d12Device, const uint64_t size[3]) {
    resourceDesc.Width = (std::max)(static_cast<UINT>(size[0]), static_cast<UINT>(1));
    resourceDesc.Height = (std::max)(static_cast<UINT>(size[1]), static_cast<UINT>(1));
    resourceDesc.DepthOrArraySize = (std::max)(static_cast<UINT>(size[2]), static_cast<UINT>(1));

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
