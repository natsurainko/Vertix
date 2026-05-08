//
// Created by Natsurainko on 2026/5/8.
//

#include "Vertix/Rendering/RenderResource.h"

void Vertix::RenderResource::Transition(
    ID3D12GraphicsCommandList* cmd,
    const D3D12_RESOURCE_STATES newState) noexcept
{
    if (currentResourceState == newState) return;

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), currentResourceState, newState);
    cmd->ResourceBarrier(1, &barrier);
    currentResourceState = newState;
}

Vertix::RenderResource::ScopedTransition Vertix::RenderResource::TransitionScoped(
    ID3D12GraphicsCommandList* cmd,
    const D3D12_RESOURCE_STATES targetState) noexcept
{
    const D3D12_RESOURCE_STATES prev = currentResourceState;
    Transition(cmd, targetState);
    return { this, cmd, prev };
}

Vertix::RenderResource::ScopedTransition Vertix::RenderResource::TransitionScoped(
    ID3D12GraphicsCommandList* cmd,
    const D3D12_RESOURCE_STATES targetState,
    const D3D12_RESOURCE_STATES restoreState) noexcept
{
    Transition(cmd, targetState);
    return { this, cmd, restoreState };
}
