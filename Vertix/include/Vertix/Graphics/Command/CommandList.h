//
// Created by Natsurainko on 2026/1/13.
//

#pragma once

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/D3D12Interface.h"

namespace Vertix {
    class CommandAllocator;

    class CommandList {
    protected:
        D3D12_COMMAND_LIST_TYPE                             commandListType;
        Microsoft::WRL::ComPtr<D3D12Interface::CommandList> commandList;

    public:
#if VERTIX_D3D12_DEVICE_VERSION >= 5
        VERTIX_API CommandList(
            ID3D12Device5*          device,
            D3D12_COMMAND_LIST_TYPE type);
#endif

        VERTIX_API CommandList(
            ID3D12Device*           device,
            const CommandAllocator* commandAllocator,
            D3D12_COMMAND_LIST_TYPE type,
            ID3D12PipelineState*    pipelineState = nullptr);

        VERTIX_API void BeginCommand(const CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState = nullptr) const;
        VERTIX_API void EndCommand() const;

        [[nodiscard]] D3D12Interface::CommandList* GetD3D12GraphicsCommandList() const noexcept { return commandList.Get(); }
        [[nodiscard]] D3D12_COMMAND_LIST_TYPE      GetD3D12GraphicsCommandListType() const noexcept { return commandListType; }
    };
}
