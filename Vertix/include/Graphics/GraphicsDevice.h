//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GRAPHICSDEVICE_H
#define VERTIX_GRAPHICSDEVICE_H

#include <dxgi1_6.h>
#include <wrl.h>

#include "d3d12/d3d12.h"

namespace Vertix {
    class GameWindow;
    class SwapChain;
    class GraphicsDevice {
    public:
        explicit GraphicsDevice(bool useSoftware = false);

        void WaitForGPU() const;

        [[nodiscard]] Microsoft::WRL::ComPtr<IDXGIFactory6> GetDxgiFactory() const;
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Device10> GetD3D12Device() const;
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

        ~GraphicsDevice();

    private:
        bool useSoftwareRendering = false;

        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
        Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

#ifndef NDEBUG
        Microsoft::WRL::ComPtr<ID3D12Debug5> d3dDebug;
#endif

        HRESULT CreateSuitableDevice();
    };
}

#endif //VERTIX_GRAPHICSDEVICE_H