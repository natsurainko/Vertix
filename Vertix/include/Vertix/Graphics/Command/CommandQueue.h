//
// Created by Natsurainko on 2026/5/26.
//

#pragma once

#include <future>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/D3D12Interface.h"

namespace Vertix {
    class CommandList;

    class CommandQueue {
        ID3D12Device*                                        device;
        const D3D12_COMMAND_QUEUE_DESC                       queueDesc;
        Microsoft::WRL::ComPtr<D3D12Interface::CommandQueue> commandQueue;

    public:
        VERTIX_API explicit CommandQueue(const Microsoft::WRL::ComPtr<D3D12Interface::CommandQueue> &commandQueue);
        VERTIX_API explicit CommandQueue(
            ID3D12Device*                   device,
            const D3D12_COMMAND_QUEUE_DESC &queueDesc);

        template <typename... Args> requires (std::same_as<Args, CommandList*> && ...)
        void ExecuteCommandLists(Args... args) {
            ID3D12CommandList* commandLists[] = { args->GetD3D12GraphicsCommandList()... };
            const size_t       count          = sizeof...(args);
            commandQueue->ExecuteCommandLists(static_cast<UINT>(count), commandLists);
        }

        VERTIX_API void WaitAllCommands() const;
        VERTIX_API auto WaitAllCommandsAsync() const -> std::future<void>;

        [[nodiscard]] D3D12Interface::CommandQueue*   GetD3D12CommandQueue() const noexcept { return commandQueue.Get(); }
        [[nodiscard]] const D3D12_COMMAND_QUEUE_DESC& GetD3D12CommandQueueDesc() const noexcept { return queueDesc; }
    };
}
