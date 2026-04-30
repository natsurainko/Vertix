//
// Created by Natsurainko on 2026/1/26.
//

#include "Vertix/Rendering/RenderTargetView.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Graphics/GraphicsDevice.h"

Vertix::RenderTargetView::RenderTargetView(
    const GraphicsDevice* graphicsDevice,
    const D3D12_RESOURCE_DESC &rtvResourceDesc,
    const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
    const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc,
    const D3D12_CLEAR_VALUE &clearValue) : d3d12Device(graphicsDevice->GetD3D12Device()), rtvHandle(descriptorHandle), rtvResourceDesc(rtvResourceDesc), clearValue(clearValue)
{
    if (rtvDesc) {
        hasRtvDesc = true;
        this->rtvDesc = *rtvDesc;
    }

    this->clearValue.Format = rtvResourceDesc.Format;

    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &rtvResourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &this->clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    d3d12Device->CreateRenderTargetView(d3d12Resource.Get(), rtvDesc, rtvHandle);
}

D3D12_RESOURCE_BARRIER Vertix::RenderTargetView::CreateTransitionBarrier(
    const D3D12_RESOURCE_STATES before,
    const D3D12_RESOURCE_STATES after,
    const UINT subresource,
    const D3D12_RESOURCE_BARRIER_FLAGS flags) const
{
    return CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), before, after, subresource, flags);
}

void Vertix::RenderTargetView::CreateShaderResourceView(
    const D3D12_SHADER_RESOURCE_VIEW_DESC *srvDesc,
    const D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) const
{
    d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), srvDesc, descriptorHandle);
}

void Vertix::RenderTargetView::Resize(const Vector2D<UINT> &size) {
    rtvResourceDesc.Width = (std::max)(size.X, static_cast<UINT>(1));
    rtvResourceDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

    d3d12Resource.Reset();

    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &rtvResourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    d3d12Device->CreateRenderTargetView(d3d12Resource.Get(), hasRtvDesc ? &rtvDesc : nullptr, rtvHandle);
}
