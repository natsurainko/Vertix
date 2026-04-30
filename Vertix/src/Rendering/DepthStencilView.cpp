//
// Created by Natsurainko on 2026/1/16.
//

#include "Vertix/Rendering/DepthStencilView.h"

#include <algorithm>
#include <d3d12/d3dx12_core.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Graphics/GraphicsDevice.h"

Vertix::DepthStencilView::DepthStencilView(
    const GraphicsDevice *graphicsDevice,
    const D3D12_RESOURCE_DESC &dsvResourceDesc,
    const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
    const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
    const D3D12_CLEAR_VALUE &clearValue) : d3d12Device(graphicsDevice->GetD3D12Device()), dsvHandle(descriptorHandle), dsvResourceDesc(dsvResourceDesc), clearValue(clearValue)
{
    if (dsvDesc) {
        hasDsvDesc = true;
        this->dsvDesc = *dsvDesc;

        if (clearValue.Format == DXGI_FORMAT_UNKNOWN) {
            this->clearValue.Format = dsvDesc->Format;
        }
    } else {
        if (clearValue.Format == DXGI_FORMAT_UNKNOWN) {
            this->clearValue.Format = dsvResourceDesc.Format;
        }
    }

    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &dsvResourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &this->clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    d3d12Device->CreateDepthStencilView(d3d12Resource.Get(), dsvDesc, dsvHandle);
}


CD3DX12_RESOURCE_BARRIER Vertix::DepthStencilView::CreateTransitionBarrier(
    const D3D12_RESOURCE_STATES before,
    const D3D12_RESOURCE_STATES after,
    const UINT subresource,
    const D3D12_RESOURCE_BARRIER_FLAGS flags) const
{
    return CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), before, after, subresource, flags);
}

void Vertix::DepthStencilView::CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC *srvDesc, const D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) const {
    d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), srvDesc, descriptorHandle);
}

void Vertix::DepthStencilView::Resize(const Vector2D<UINT> &size) {
    dsvResourceDesc.Width = (std::max)(size.X, static_cast<UINT>(1));
    dsvResourceDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

    d3d12Resource.Reset();

    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &dsvResourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    d3d12Device->CreateDepthStencilView(d3d12Resource.Get(), hasDsvDesc ? &dsvDesc : nullptr, dsvHandle);
}
