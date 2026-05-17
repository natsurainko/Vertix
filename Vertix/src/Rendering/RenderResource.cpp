//
// Created by Natsurainko on 2026/5/8.
//

#include "Vertix/Rendering/RenderResource.h"

#include <d3d12/d3dx12_barriers.h>

void Vertix::RenderResource::Transition(
    ID3D12GraphicsCommandList* cmd,
    const D3D12_RESOURCE_STATES newState) noexcept
{
    if (currentResourceState == newState) return;

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), currentResourceState, newState);
    cmd->ResourceBarrier(1, &barrier);
    currentResourceState = newState;
}
