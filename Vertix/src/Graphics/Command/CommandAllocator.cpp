//
// Created by Natsurainko on 2026/5/28.
//

#include "Vertix/Graphics/Command/CommandAllocator.h"

Vertix::CommandAllocator::CommandAllocator(
    ID3D12Device*                 device,
    const D3D12_COMMAND_LIST_TYPE commandListType) : commandListType(commandListType) {
    ThrowIfFailed(device->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&commandAllocator)));
}

void Vertix::CommandAllocator::Reset() const {
    ThrowIfFailed(commandAllocator->Reset());
}
