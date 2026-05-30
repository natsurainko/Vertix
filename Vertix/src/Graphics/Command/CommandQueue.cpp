//
// Created by Natsurainko on 2026/5/26.
//

#include "Vertix/Graphics/Command/CommandQueue.h"

Vertix::CommandQueue::CommandQueue(const Microsoft::WRL::ComPtr<D3D12Interface::CommandQueue> &commandQueue)
: queueDesc(commandQueue->GetDesc()),
  commandQueue(commandQueue) {
    Microsoft::WRL::ComPtr<ID3D12Device> comPtr;
    ThrowIfFailed(commandQueue->GetDevice(IID_PPV_ARGS(&comPtr)));
    device = comPtr.Get();
}

Vertix::CommandQueue::CommandQueue(
    ID3D12Device*                   device,
    const D3D12_COMMAND_QUEUE_DESC &queueDesc) : device(device),
                                                 queueDesc(queueDesc) {
    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
}

void Vertix::CommandQueue::WaitAllCommands() const {
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    const auto fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));

    ThrowIfFailed(commandQueue->Signal(fence.Get(), 1));
    ThrowIfFailed(fence->SetEventOnCompletion(1, fenceEvent));

    WaitForSingleObject(fenceEvent, INFINITE);
    CloseHandle(fenceEvent);
}

std::future<void> Vertix::CommandQueue::WaitAllCommandsAsync() const {
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    const auto fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));

    ThrowIfFailed(commandQueue->Signal(fence.Get(), 1));
    ThrowIfFailed(fence->SetEventOnCompletion(1, fenceEvent));

    return std::async(
        std::launch::async,
        // Capture fence to prevent Microsoft::WRL::ComPtr<ID3D12Fence> destructing
        [fence, fenceEvent] {
            WaitForSingleObject(fenceEvent, INFINITE);
            CloseHandle(fenceEvent);
        }
    );
}
