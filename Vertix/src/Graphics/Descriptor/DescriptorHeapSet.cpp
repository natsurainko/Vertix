//
// Created by Natsurainko on 2026/5/21.
//

#include "Vertix/Graphics/Descriptor/DescriptorHeapSet.h"

Vertix::DescriptorHeapSet::DescriptorHeapSet(ID3D12Device* device, const uint32_t heapsCapacity[4]) : device(device) {
    for (auto type = 0; type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++type) {
        const auto heapType       = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type);
        const auto shaderVisible  = heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        descriptorHeaps[heapType] = std::make_unique<DescriptorHeap>(
            device,
            heapType,
            heapsCapacity[type],
            shaderVisible
        );

        if (shaderVisible) {
            gpuHeaps[heapType] = descriptorHeaps[heapType]->GetDescriptorHeap();
        }
    }
}

Vertix::DescriptorHandle Vertix::DescriptorHeapSet::CreateRTV(
    ID3D12Resource*                                     resource,
    const std::optional<D3D12_RENDER_TARGET_VIEW_DESC> &desc) const {
    const auto handle = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->AllocDescriptorHandle();
    device->CreateRenderTargetView(resource, desc.has_value() ? &desc.value() : nullptr, handle.cpuHandle);
    return handle;
}

Vertix::DescriptorHandle Vertix::DescriptorHeapSet::CreateDSV(
    ID3D12Resource*                                     resource,
    const std::optional<D3D12_DEPTH_STENCIL_VIEW_DESC> &desc) const {
    const auto handle = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->AllocDescriptorHandle();
    device->CreateDepthStencilView(resource, desc.has_value() ? &desc.value() : nullptr, handle.cpuHandle);
    return handle;
}

Vertix::DescriptorHandle Vertix::DescriptorHeapSet::CreateCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC &desc) const {
    const auto handle = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->AllocDescriptorHandle();
    device->CreateConstantBufferView(&desc, handle.cpuHandle);
    return handle;
}

Vertix::DescriptorHandle Vertix::DescriptorHeapSet::CreateUAV(
    ID3D12Resource*                                        resource,
    const std::optional<D3D12_UNORDERED_ACCESS_VIEW_DESC> &desc, ID3D12Resource* counterResource) const {
    const auto handle = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->AllocDescriptorHandle();
    device->CreateUnorderedAccessView(resource, counterResource, desc.has_value() ? &desc.value() : nullptr, handle.cpuHandle);
    return handle;
}

Vertix::DescriptorHandle Vertix::DescriptorHeapSet::CreateSRV(
    ID3D12Resource*                                       resource,
    const std::optional<D3D12_SHADER_RESOURCE_VIEW_DESC> &desc) const {
    const auto handle = descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->AllocDescriptorHandle();
    device->CreateShaderResourceView(resource, desc.has_value() ? &desc.value() : nullptr, handle.cpuHandle);
    return handle;
}
