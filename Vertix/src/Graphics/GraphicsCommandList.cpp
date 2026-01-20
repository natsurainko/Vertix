//
// Created by Natsurainko on 2026/1/13.
//

#include "Graphics/GraphicsCommandList.h"

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

using Microsoft::WRL::ComPtr;

Vertix::GraphicsCommandList::GraphicsCommandList(const GraphicsDevice *graphicsDevice) {
    commandQueue = graphicsDevice->GetD3D12CommandQueue();
    d3d12Device = graphicsDevice->GetD3D12Device();

    {
        ThrowIfFailed(d3d12Device->CreateCommandList1(
           0,
           D3D12_COMMAND_LIST_TYPE_DIRECT,
           D3D12_COMMAND_LIST_FLAG_NONE,
           IID_PPV_ARGS(&graphicsCommandList)));
    }
    {
        ThrowIfFailed(d3d12Device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&commandAllocator)));
    }
}

void Vertix::GraphicsCommandList::BeginCommand(const ComPtr<ID3D12PipelineState> &pipelineState) const {
    ThrowIfFailed(commandAllocator->Reset());
    ThrowIfFailed(graphicsCommandList->Reset(commandAllocator.Get(), pipelineState.Get()));
}

void Vertix::GraphicsCommandList::EndCommand() const {
    ThrowIfFailed(graphicsCommandList->Close());
    ID3D12CommandList* commandLists[] = { graphicsCommandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void Vertix::GraphicsCommandList::WaitForCommand() const {
    ComPtr<ID3D12Fence> fence;
    ThrowIfFailed(d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    const HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr) {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    ThrowIfFailed(commandQueue->Signal(fence.Get(), 1));
    ThrowIfFailed(fence->SetEventOnCompletion(1, fenceEvent));

    WaitForSingleObject(fenceEvent, INFINITE);
    CloseHandle(fenceEvent);
}
