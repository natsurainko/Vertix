//
// Created by Natsurainko on 2026/5/16.
//

#include "Vertix/Rendering/RenderTexture.h"

std::unique_ptr<Vertix::RenderTexture1D> Vertix::RenderTexture::Tex1D(
    ID3D12Device*                           device,
    const RenderResourceUsage               usage,
    const D3D12_RESOURCE_DESC &             resourceDesc,
    const std::optional<D3D12_CLEAR_VALUE> &clearValue) {
    auto desc  = resourceDesc;
    desc.Flags = DeriveResourceFlags(usage);
    assert(desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D);

    const auto heapProps    = DeriveHeapProperties(usage);
    const auto initialState = DeriveState(usage);

    Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
    ThrowIfFailed(
        device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            initialState,
            clearValue.has_value() ? &clearValue.value() : nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        )
    );

    return std::make_unique<RenderTexture1D>(d3d12Resource, desc, initialState, clearValue);
}

std::unique_ptr<Vertix::RenderTexture2D> Vertix::RenderTexture::Tex2D(
    ID3D12Device*                           device,
    const RenderResourceUsage               usage,
    const D3D12_RESOURCE_DESC &             resourceDesc,
    const std::optional<D3D12_CLEAR_VALUE> &clearValue) {
    auto desc  = resourceDesc;
    desc.Flags = DeriveResourceFlags(usage);
    assert(desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D);

    const auto heapProps    = DeriveHeapProperties(usage);
    const auto initialState = DeriveState(usage);

    Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
    ThrowIfFailed(
        device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            initialState,
            clearValue.has_value() ? &clearValue.value() : nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        )
    );

    return std::make_unique<RenderTexture2D>(d3d12Resource, desc, initialState, clearValue);
}

std::unique_ptr<Vertix::RenderTexture3D> Vertix::RenderTexture::Tex3D(
    ID3D12Device*                           device,
    const RenderResourceUsage               usage,
    const D3D12_RESOURCE_DESC &             resourceDesc,
    const std::optional<D3D12_CLEAR_VALUE> &clearValue) {
    auto desc  = resourceDesc;
    desc.Flags = DeriveResourceFlags(usage);
    assert(desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D);

    const auto heapProps    = DeriveHeapProperties(usage);
    const auto initialState = DeriveState(usage);

    Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
    ThrowIfFailed(
        device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            initialState,
            clearValue.has_value() ? &clearValue.value() : nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        )
    );

    return std::make_unique<RenderTexture3D>(d3d12Resource, desc, initialState, clearValue);
}

void Vertix::RenderTexture1D::Resize(ID3D12Device* d3d12Device, const uint64_t size[3]) {
    resourceDesc.Width = (std::max)(size[0], static_cast<uint64_t>(1));

    d3d12Resource.Reset();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(
        d3d12Device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            currentResourceState,
            clearValue.has_value() ? &clearValue.value() : nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        )
    );
}

void Vertix::RenderTexture2D::Resize(ID3D12Device* d3d12Device, const uint64_t size[3]) {
    resourceDesc.Width  = (std::max)(static_cast<UINT>(size[0]), static_cast<UINT>(1));
    resourceDesc.Height = (std::max)(static_cast<UINT>(size[1]), static_cast<UINT>(1));

    d3d12Resource.Reset();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(
        d3d12Device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            currentResourceState,
            clearValue.has_value() ? &clearValue.value() : nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        )
    );
}

void Vertix::RenderTexture2D::Resize(
    ID3D12Device*        d3d12Device,
    const Vector2D<UINT> size) {
    resourceDesc.Width  = (std::max)(size.X, static_cast<UINT>(1));
    resourceDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

    d3d12Resource.Reset();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(
        d3d12Device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            currentResourceState,
            clearValue.has_value() ? &clearValue.value() : nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        )
    );
}

void Vertix::RenderTexture3D::Resize(ID3D12Device* d3d12Device, const uint64_t size[3]) {
    resourceDesc.Width            = (std::max)(static_cast<UINT>(size[0]), static_cast<UINT>(1));
    resourceDesc.Height           = (std::max)(static_cast<UINT>(size[1]), static_cast<UINT>(1));
    resourceDesc.DepthOrArraySize = (std::max)(static_cast<UINT>(size[2]), static_cast<UINT>(1));

    d3d12Resource.Reset();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(
        d3d12Device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            currentResourceState,
            clearValue.has_value() ? &clearValue.value() : nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        )
    );
}
