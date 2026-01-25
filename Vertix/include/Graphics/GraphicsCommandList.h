//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_COMMANDLIST_H
#define VERTIX_COMMANDLIST_H

#include <d3d12/d3d12.h>
#include <wrl/client.h>

namespace Vertix {
    class GraphicsDevice;
    class GraphicsCommandList {
    public:
        explicit GraphicsCommandList(const GraphicsDevice* graphicsDevice);
        virtual ~GraphicsCommandList() = default;

        virtual void BeginCommand(const Microsoft::WRL::ComPtr<ID3D12PipelineState> &pipelineState) const;
        virtual void EndCommand() const;
        void WaitForCommand() const;

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> GetD3D12GraphicsCommandList() const {
            return graphicsCommandList;
        }
    protected:
        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> graphicsCommandList;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    };
}

#endif //VERTIX_COMMANDLIST_H