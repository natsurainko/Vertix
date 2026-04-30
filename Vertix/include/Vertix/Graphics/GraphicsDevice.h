//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GRAPHICSDEVICE_H
#define VERTIX_GRAPHICSDEVICE_H

#include <dxgi1_6.h>
#include <wrl.h>
#include <d3d12/d3d12.h>

#include "Vertix/VERTIX_EXPORT.h"

namespace Vertix {
    struct GraphicsDeviceOptions {
        bool useSoftware = false;
        bool enableDebugLayer = false;
    };

    class GameWindow;
    class SwapChain;
    class GraphicsDevice {
    public:
        VERTIX_API explicit GraphicsDevice(
            bool useSoftware = false,
            bool enableDebugLayer = false);

        VERTIX_API ~GraphicsDevice();

        VERTIX_API void CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12CommandQueue> &commandQueue, const D3D12_COMMAND_QUEUE_DESC &queueDesc) const;
        VERTIX_API void WaitForGPU() const;

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<IDXGIFactory6>& GetDxgiFactory() const noexcept {
            return dxgiFactory;
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Device10>& GetD3D12Device() const noexcept {
            return d3d12Device;
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetDefaultD3D12CommandQueue() const noexcept {
            return d3d12CommandQueue;
        }

        [[nodiscard]]
        bool IsTypedUAVLoadAdditionalFormatsSupported() const noexcept {
            return d3d12FeatureOptions.TypedUAVLoadAdditionalFormats;
        }

        [[nodiscard]]
        bool IsStandardSwizzle64KBSupported() const noexcept {
            return d3d12FeatureOptions.StandardSwizzle64KBSupported;
        }

    private:
        bool useSoftwareRendering = false;

        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12Debug5> d3dDebug;
        Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

        D3D12_FEATURE_DATA_D3D12_OPTIONS d3d12FeatureOptions{};

        HRESULT CreateSuitableDevice();
    };
}

#endif //VERTIX_GRAPHICSDEVICE_H
