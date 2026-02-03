//
// Created by Natsurainko on 2026/1/16.
//

#include "Rendering/DepthStencilView.h"

#include <algorithm>
#include <d3d12/d3dx12_core.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

Vertix::DepthStencilView::DepthStencilView(GraphicsDevice *graphicsDevice,
                                           const D3D12_RESOURCE_DESC &dsvResourceDesc,
                                           const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
                                           const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
                                           const D3D12_CLEAR_VALUE &clearValue)
    : graphicsDevice(graphicsDevice), dsvHandle(descriptorHandle), dsvResourceDesc(dsvResourceDesc), clearValue(clearValue) {


    if (dsvDesc) {
        hasDsvDesc = true;
        this->depthStencilViewDesc = *dsvDesc;

        if (clearValue.Format == DXGI_FORMAT_UNKNOWN) {
            this->clearValue.Format = dsvDesc->Format;
        }
    } else {
        if (clearValue.Format == DXGI_FORMAT_UNKNOWN) {
            this->clearValue.Format = dsvResourceDesc.Format;
        }
    }

    const auto &device = graphicsDevice->GetD3D12Device();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &dsvResourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &this->clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    device->CreateDepthStencilView(d3d12Resource.Get(), dsvDesc, dsvHandle);
}

void Vertix::DepthStencilView::Resize(const Vector2D<UINT> &size) {
    dsvResourceDesc.Width = (std::max)(size.X, static_cast<UINT>(1));
    dsvResourceDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

    d3d12Resource.Reset();
    const auto &device = graphicsDevice->GetD3D12Device();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &dsvResourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    device->CreateDepthStencilView(d3d12Resource.Get(), hasDsvDesc ? &depthStencilViewDesc : nullptr, dsvHandle);
}