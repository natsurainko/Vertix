//
// Created by Natsurainko on 2026/5/28.
//

#pragma once

#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "Vertix/D3D12Interface.h"

namespace Vertix {
    class CommandAllocator {
        D3D12_COMMAND_LIST_TYPE                                  commandListType;
        Microsoft::WRL::ComPtr<D3D12Interface::CommandAllocator> commandAllocator;

    public:
        VERTIX_API explicit CommandAllocator(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandListType);
        virtual             ~CommandAllocator() = default;

        VERTIX_API void Reset() const;

        [[nodiscard]] virtual D3D12Interface::CommandAllocator* GetD3D12CommandAllocator() const noexcept { return commandAllocator.Get(); }
        [[nodiscard]] D3D12_COMMAND_LIST_TYPE                   GetD3D12CommandAllocatorType() const noexcept { return commandListType; }
    };
}
