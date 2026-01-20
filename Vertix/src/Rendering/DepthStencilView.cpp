//
// Created by Natsurainko on 2026/1/16.
//

#include "Rendering/DepthStencilView.h"

#include "d3d12/d3dx12_core.h"
#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

Vertix::DepthStencilView::DepthStencilView(GraphicsDevice *graphicsDevice,
                                           const D3D12_RESOURCE_DESC &depthStencilDesc,
                                           const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
                                           const D3D12_DEPTH_STENCIL_VIEW_DESC* depthStencilViewDesc)
        : graphicsDevice(graphicsDevice), dsvHandle(descriptorHandle), depthStencilDesc(depthStencilDesc) {
    if (depthStencilViewDesc) {
        hasDepthStencilViewDesc = true;
        this->depthStencilViewDesc = *depthStencilViewDesc;
    }

    clearValue.Format = depthStencilDesc.Format;

    const auto device = graphicsDevice->GetD3D12Device();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    device->CreateDepthStencilView(d3d12Resource.Get(), depthStencilViewDesc, dsvHandle);
}

void Vertix::DepthStencilView::Resize(const Vector2D<UINT> &size) {
    depthStencilDesc.Width = size.X;
    depthStencilDesc.Height = size.Y;

    d3d12Resource.Reset();
    const auto device = graphicsDevice->GetD3D12Device();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    device->CreateDepthStencilView(d3d12Resource.Get(), hasDepthStencilViewDesc ? &depthStencilViewDesc : nullptr, dsvHandle);
}