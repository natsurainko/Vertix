//
// Created by Natsurainko on 2026/5/29.
//

#include "Vertix/Rendering/Buffers/StructuredBuffer.h"

Vertix::StructuredBufferBase::StructuredBufferBase(
    const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
    const D3D12_RESOURCE_STATES                   currentResourceState,
    const Microsoft::WRL::ComPtr<ID3D12Resource> &stagingBuffer,
    const uint32_t                                elementCount,
    const uint32_t                                elementSize)
: RenderBuffer(d3d12Resource, currentResourceState),
  stagingBuffer(stagingBuffer),
  elementCount(elementCount),
  elementSize(elementSize) {
    const CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(
        stagingBuffer->Map(
            0,
            &readRange,
            reinterpret_cast<void**>(&stagingDataBegin)
        )
    );
}

Vertix::StructuredBufferBase::~StructuredBufferBase() {
    if (stagingDataBegin) {
        stagingBuffer->Unmap(0, nullptr);
        stagingDataBegin = nullptr;
    }
}

void Vertix::StructuredBufferBase::FillRaw(
    ID3D12GraphicsCommandList* cmdList,
    const uint32_t             index,
    const void*                data,
    const uint32_t             offset,
    const uint32_t             size) {
    assert(stagingDataBegin && "Invalid bufferDataPtr");
    assert(index < elementCount && "Index out of range");
    assert(offset + size <= elementSize && "FillRaw out of bounds");

    const uint64_t byteOffset = index * elementSize + offset;
    memcpy(stagingDataBegin + byteOffset, data, size);

    const auto preState = currentResourceState;
    Transition(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);
    {
        cmdList->CopyBufferRegion(
            d3d12Resource.Get(),
            byteOffset,
            stagingBuffer.Get(),
            byteOffset,
            size
        );
    }
    Transition(cmdList, preState);
}

void Vertix::StructuredBufferBase::FillRawRange(
    ID3D12GraphicsCommandList* cmdList,
    const uint32_t             startIndex,
    const uint32_t             count,
    const void*                data) {
    assert(stagingDataBegin && "Staging buffer not mapped");
    assert(count > 0 && "Count must be > 0");
    assert(startIndex + count <= elementCount && "FillRawRange out of bounds");

    const uint64_t dstOffset  = startIndex * elementSize;
    const uint64_t totalBytes = count * elementSize;
    memcpy(stagingDataBegin + dstOffset, data, totalBytes);

    const auto preState = currentResourceState;
    Transition(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);
    {
        cmdList->CopyBufferRegion(
            d3d12Resource.Get(),
            dstOffset,
            stagingBuffer.Get(),
            dstOffset,
            totalBytes
        );
    }
    Transition(cmdList, preState);
}
