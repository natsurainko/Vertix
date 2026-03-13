//
// Created by Natsurainko on 2025/12/24.
//

#include "Graphics/FrameCommandList.h"

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"

using Microsoft::WRL::ComPtr;

Vertix::FrameCommandList::FrameCommandList(
    const GraphicsDevice* graphicsDevice,
    const UINT frameCount) : GraphicsCommandList(graphicsDevice->GetD3D12Device(), graphicsDevice->GetDefaultD3D12CommandQueue()), frameCount(frameCount)
{
    if (frameCount == 0) throw std::exception("frameCount cannot be zero");

    allocators = std::vector<ComPtr<ID3D12CommandAllocator>>(frameCount);
    allocators[0] = commandAllocator;

    for (UINT i = 1; i < frameCount; ++i) {
        ThrowIfFailed(d3d12Device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&allocators[i])));
    }

    fenceValues.resize(frameCount);
    ThrowIfFailed(d3d12Device->CreateFence(
        fenceValues[currentFrameIndex],
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&fence)));
    fenceValues[currentFrameIndex]++;

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr) {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }
}

Vertix::FrameCommandList::~FrameCommandList() {
    if (fenceEvent) {
        CloseHandle(fenceEvent);
    }
}

void Vertix::FrameCommandList::BeginCommand(const ComPtr<ID3D12PipelineState> &pipelineState) const {
    const auto allocator = allocators[currentFrameIndex];
    ThrowIfFailed(allocator->Reset());
    ThrowIfFailed(graphicsCommandList->Reset(allocator.Get(), pipelineState.Get()));
}

// Prepare to render the next frame.
void Vertix::FrameCommandList::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = fenceValues[currentFrameIndex];
    ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFenceValue));

    // Update the frame index.
    currentFrameIndex = (currentFrameIndex + 1) % frameCount;

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (fence->GetCompletedValue() < fenceValues[currentFrameIndex]) {
        ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[currentFrameIndex], fenceEvent));
        WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    fenceValues[currentFrameIndex] = currentFenceValue + 1;
}

void Vertix::FrameCommandList::WaitForCommand() {
    ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValues[currentFrameIndex]));
    ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[currentFrameIndex], fenceEvent));
    WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
    fenceValues[currentFrameIndex]++;
}
