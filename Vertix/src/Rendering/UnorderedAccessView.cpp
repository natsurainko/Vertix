//
// Created by Natsurainko on 2026/3/28.
//

#include "Vertix/Rendering/UnorderedAccessView.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Graphics/GraphicsDevice.h"

Vertix::UnorderedAccessView::UnorderedAccessView(
    const GraphicsDevice* graphicsDevice,
    const D3D12_RESOURCE_DESC &uavResourceDesc,
    const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
    const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) : d3d12Device(graphicsDevice->GetD3D12Device()), uavHandle(descriptorHandle), uavResourceDesc(uavResourceDesc)
{
    if (uavDesc) {
        hasUavDesc = true;
        this->uavDesc = *uavDesc;
    }

    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uavResourceDesc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    d3d12Device->CreateUnorderedAccessView(d3d12Resource.Get(), nullptr, uavDesc, descriptorHandle);
}

D3D12_RESOURCE_BARRIER Vertix::UnorderedAccessView::CreateTransitionBarrier(
    const D3D12_RESOURCE_STATES before,
    const D3D12_RESOURCE_STATES after,
    const UINT subresource,
    const D3D12_RESOURCE_BARRIER_FLAGS flags) const
{
    return CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), before, after, subresource, flags);
}

void Vertix::UnorderedAccessView::CreateShaderResourceView(
    const D3D12_SHADER_RESOURCE_VIEW_DESC *srvDesc,
    const D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) const
{
    d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), srvDesc, descriptorHandle);
}

void Vertix::UnorderedAccessView::Resize(const Vector2D<UINT> &size) {
    uavResourceDesc.Width = (std::max)(size.X, static_cast<UINT>(1));
    uavResourceDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

    d3d12Resource.Reset();

    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uavResourceDesc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    d3d12Device->CreateUnorderedAccessView(d3d12Resource.Get(), nullptr, hasUavDesc ? &uavDesc : nullptr, uavHandle);
}
