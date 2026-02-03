//
// Created by Natsurainko on 2026/1/26.
//

#include "Rendering/RenderTargetView.h"

#include <d3d12/d3dx12_core.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

Vertix::RenderTargetView::RenderTargetView(GraphicsDevice* graphicsDevice,
                                           const D3D12_RESOURCE_DESC &rtvResourceDesc,
                                           const D3D12_CPU_DESCRIPTOR_HANDLE &descriptorHandle,
                                           const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc,
                                           const D3D12_CLEAR_VALUE &clearValue)
    : graphicsDevice(graphicsDevice), rtvHandle(descriptorHandle), rtvResourceDesc(rtvResourceDesc), clearValue(clearValue) {
    if (rtvDesc) {
        hasRtvDesc = true;
        this->rtvDesc = *rtvDesc;
    }

    this->clearValue.Format = rtvResourceDesc.Format;

    const auto &device = graphicsDevice->GetD3D12Device();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &rtvResourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &this->clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    device->CreateRenderTargetView(d3d12Resource.Get(), rtvDesc, rtvHandle);
}

void Vertix::RenderTargetView::Resize(const Vector2D<UINT> &size) {
    rtvResourceDesc.Width = (std::max)(size.X, static_cast<UINT>(1));
    rtvResourceDesc.Height = (std::max)(size.Y, static_cast<UINT>(1));

    d3d12Resource.Reset();
    const auto &device = graphicsDevice->GetD3D12Device();
    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &rtvResourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&d3d12Resource)
    ));

    device->CreateRenderTargetView(d3d12Resource.Get(), hasRtvDesc ? &rtvDesc : nullptr, rtvHandle);
}