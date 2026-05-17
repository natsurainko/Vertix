//
// Created by Natsurainko on 2026/5/16.
//

#include "Vertix/Rendering/RenderResourceViewAllocator.h"

#include <stdexcept>

void Vertix::RenderResourceViewAllocator::InitRenderTargetDescriptorHeap(const UINT maxDescriptors) {
    rtvDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, maxDescriptors, false);
}

void Vertix::RenderResourceViewAllocator::InitDepthStencilDescriptorHeap(const UINT maxDescriptors) {
    dsvDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, maxDescriptors, false);
}

void Vertix::RenderResourceViewAllocator::InitSharedDescriptorHeap(const UINT maxDescriptors) {
    sharedDescriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, maxDescriptors, true);
}

Vertix::DescriptorHeapHandle Vertix::RenderResourceViewAllocator::CreateView(
    const RenderResource *resource,
    const RenderResourceViewDesc &desc,
    const RenderResource *counterResource) const
{
    DescriptorHeap* heap = nullptr;

    switch (desc.type) {
        case RenderResourceViewType::RenderTarget: heap = rtvDescriptorHeap.get(); break;
        case RenderResourceViewType::DepthStencil: heap = dsvDescriptorHeap.get(); break;
        default: heap = sharedDescriptorHeap.get(); break;
    }

    const DescriptorHeapHandle handle = heap->AllocDescriptorHandle();

    if (std::holds_alternative<std::monostate>(desc.desc)) {
        switch (desc.type) {
            case RenderResourceViewType::RenderTarget:    d3d12Device->CreateRenderTargetView(resource->GetResource(), nullptr, handle.cpuHandle); break;
            case RenderResourceViewType::DepthStencil:    d3d12Device->CreateDepthStencilView(resource->GetResource(), nullptr, handle.cpuHandle); break;
            case RenderResourceViewType::UnorderedAccess: d3d12Device->CreateUnorderedAccessView(resource->GetResource(), nullptr, nullptr, handle.cpuHandle); break;
            case RenderResourceViewType::ShaderResource:  d3d12Device->CreateShaderResourceView(resource->GetResource(), nullptr, handle.cpuHandle); break;
            case RenderResourceViewType::ConstantBuffer:  throw std::runtime_error("Unreachable"); break;
        }
        return handle;
    }

    const void* descPtr = &desc.desc;
    switch (desc.type) {
        case RenderResourceViewType::RenderTarget:
            d3d12Device->CreateRenderTargetView(resource->GetResource(), static_cast<const D3D12_RENDER_TARGET_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
        case RenderResourceViewType::DepthStencil:
            d3d12Device->CreateDepthStencilView(resource->GetResource(), static_cast<const D3D12_DEPTH_STENCIL_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
        case RenderResourceViewType::UnorderedAccess:
            d3d12Device->CreateUnorderedAccessView(resource->GetResource(), counterResource ? counterResource->GetResource() : nullptr, static_cast<const D3D12_UNORDERED_ACCESS_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
        case RenderResourceViewType::ShaderResource:
            d3d12Device->CreateShaderResourceView(resource->GetResource(), static_cast<const D3D12_SHADER_RESOURCE_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
        case RenderResourceViewType::ConstantBuffer:
            d3d12Device->CreateConstantBufferView(static_cast<const D3D12_CONSTANT_BUFFER_VIEW_DESC*>(descPtr), handle.cpuHandle); break;
    }

    return handle;
}
