//
// Created by Natsurainko on 2026/1/13.
//

#include "Vertix/Graphics/Command/CommandList.h"

#include "Vertix/Graphics/Command/CommandAllocator.h"

#if VERTIX_D3D12_DEVICE_VERSION >= 5
Vertix::CommandList::CommandList(
    ID3D12Device5*                device,
    const D3D12_COMMAND_LIST_TYPE type) : commandListType(type) {
    ThrowIfFailed(
        device->CreateCommandList1(
            0,
            type,
            D3D12_COMMAND_LIST_FLAG_NONE,
            IID_PPV_ARGS(&commandList)
        )
    );
}
#endif

Vertix::CommandList::CommandList(
    ID3D12Device*                 device,
    const CommandAllocator*       commandAllocator,
    const D3D12_COMMAND_LIST_TYPE type,
    ID3D12PipelineState*          pipelineState) : commandListType(type) {
    ThrowIfFailed(
        device->CreateCommandList(
            0,
            type,
            commandAllocator->GetD3D12CommandAllocator(),
            pipelineState,
            IID_PPV_ARGS(&commandList)
        )
    );
}

void Vertix::CommandList::BeginCommand(const CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState) const {
    commandAllocator->Reset();
    ThrowIfFailed(commandList->Reset(commandAllocator->GetD3D12CommandAllocator(), pipelineState));
}

void Vertix::CommandList::EndCommand() const {
    ThrowIfFailed(commandList->Close());
}
