//
// Created by Natsurainko on 2025/12/23.
//

#pragma once

#include <d3d12/d3dx12_check_feature_support.h>

#include "Vertix/D3D12Interface.h"

namespace Vertix {
    struct GraphicsDeviceOptions {
        bool              forceSoftwareAdapter = false;
        D3D_FEATURE_LEVEL minimumFeatureLevel  = D3D_FEATURE_LEVEL_12_0;

        constexpr GraphicsDeviceOptions() = default;

        consteval GraphicsDeviceOptions(
            const bool              forceSoftwareAdapter,
            const D3D_FEATURE_LEVEL level) : forceSoftwareAdapter(forceSoftwareAdapter),
                                             minimumFeatureLevel(level) {
            if (level < D3D_FEATURE_LEVEL_12_0)
                throw std::exception("minimumFeatureLevel cannot be below D3D_FEATURE_LEVEL_12_0");
        }
    };

    class CommandAllocator;
    class CommandList;
    class CommandQueue;
    class SwapChain;

    class GraphicsDevice {
        Microsoft::WRL::ComPtr<D3D12Interface::Device>      device;
        Microsoft::WRL::ComPtr<D3D12Interface::DXGIFactory> dxgiFactory;

#if defined(_DEBUG) && defined(VERTIX_D3D12_ENABLE_DEBUG_LAYER)
        Microsoft::WRL::ComPtr<D3D12Interface::Debug>       debug;
        Microsoft::WRL::ComPtr<D3D12Interface::DXGIDebug>   dxgiDebug;
        Microsoft::WRL::ComPtr<D3D12Interface::DebugDevice> debugDevice;
#endif

        GraphicsDeviceOptions initialOptions;
        CD3DX12FeatureSupport featureSupport = {};

    public:
        VERTIX_API explicit GraphicsDevice(const GraphicsDeviceOptions &options);
        VERTIX_API          ~GraphicsDevice();

        [[nodiscard]] VERTIX_API std::unique_ptr<CommandQueue>     CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC &queueDesc) const;
        [[nodiscard]] VERTIX_API std::unique_ptr<CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type) const;
        [[nodiscard]] VERTIX_API std::unique_ptr<CommandList>      CreateCommandList(D3D12_COMMAND_LIST_TYPE type) const;

        [[nodiscard]] VERTIX_API std::unique_ptr<SwapChain> CreateSwapChain(
            const CommandQueue*          commandQueue,
            HWND                         hWnd,
            const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc) const;

        [[nodiscard]] D3D12Interface::Device*      GetD3D12Device() const noexcept { return device.Get(); }
        [[nodiscard]] D3D12Interface::DXGIFactory* GetDxgiFactory() const noexcept { return dxgiFactory.Get(); }

        [[nodiscard]] const CD3DX12FeatureSupport& GetFeatureSupport() const noexcept { return featureSupport; }
        [[nodiscard]] const GraphicsDeviceOptions& GetInitialOptions() const noexcept { return initialOptions; }

    private:
        HRESULT EnumerateAdapters(Microsoft::WRL::ComPtr<D3D12Interface::DXGIAdapter> &adaptor) const;
        HRESULT CreateDevice(const Microsoft::WRL::ComPtr<D3D12Interface::DXGIAdapter> &adaptor);

#if defined(_DEBUG) && defined(VERTIX_D3D12_ENABLE_DEBUG_LAYER)
        void EnableDebugLayer(uint32_t &dxgiFactoryFlags);
        void ReportLiveObjects();
#endif
    };
}
