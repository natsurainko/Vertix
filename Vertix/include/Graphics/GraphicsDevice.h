//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_GRAPHICSDEVICE_H
#define VERTIX_GRAPHICSDEVICE_H

#include <dxgi1_6.h>
#include <wrl.h>
#include <d3d12/d3d12.h>

namespace Vertix {
    class GameWindow;
    class SwapChain;
    class GraphicsDevice {
    public:
        explicit GraphicsDevice(bool useSoftware = false);
        ~GraphicsDevice();

        void WaitForGPU() const;

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<IDXGIFactory6>& GetDxgiFactory() const {
            return dxgiFactory;
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Device10>& GetD3D12Device() const {
            return d3d12Device;
        }

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetDefaultD3D12CommandQueue() const {
            return d3d12CommandQueue;
        }

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