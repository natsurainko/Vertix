//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_COMMANDLIST_H
#define VERTIX_COMMANDLIST_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/VERTIX_EXPORT.h"

namespace Vertix {
    class GraphicsDevice;
    class GraphicsCommandList {
    public:
        VERTIX_API GraphicsCommandList(
            const Microsoft::WRL::ComPtr<ID3D12Device10> &d3d12Device,
            const Microsoft::WRL::ComPtr<ID3D12CommandQueue> &commandQueue,
            D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT);

        VERTIX_API virtual ~GraphicsCommandList() = default;

        VERTIX_API virtual void BeginCommand(const Microsoft::WRL::ComPtr<ID3D12PipelineState> &pipelineState) const;
        VERTIX_API virtual void EndCommand() const;
        VERTIX_API virtual void WaitForCommand();

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5>& GetD3D12GraphicsCommandList() const noexcept {
            return graphicsCommandList;
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetD3D12CommandQueue() const noexcept {
            return commandQueue;
        }

    protected:
        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> graphicsCommandList;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
        D3D12_COMMAND_LIST_TYPE commandListType;
    };
}

#endif //VERTIX_COMMANDLIST_H
