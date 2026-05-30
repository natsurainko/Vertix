//
// Created by Natsurainko on 2026/5/29.
//

#include "Vertix/Rendering/Buffers/ConstantBuffer.h"

#include <cassert>

Vertix::ConstantBufferBase::ConstantBufferBase(
    const Microsoft::WRL::ComPtr<ID3D12Resource> &resource,
    const D3D12_RESOURCE_STATES                   currentResourceState,
    const uint32_t                                dataSize)
: RenderBuffer(resource, currentResourceState),
  dataSize(dataSize) {
    const CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(
        resource->Map(
            0,
            &readRange,
            reinterpret_cast<void**>(&bufferDataBegin)
        )
    );
}

Vertix::ConstantBufferBase::~ConstantBufferBase() {
    if (bufferDataBegin) {
        d3d12Resource->Unmap(0, nullptr);
        bufferDataBegin = nullptr;
    }
}

void Vertix::ConstantBufferBase::FillRaw(
    const void*    data,
    const uint32_t offset,
    const uint32_t size) const {
    assert(offset + size <= dataSize && "FillRaw out of bounds");
    memcpy(bufferDataBegin + offset, data, size);
}
