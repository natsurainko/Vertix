//
// Created by Natsurainko on 2025/12/23.
//

#include "Vertix/Graphics/GraphicsDevice.h"

#include "Vertix/Graphics/Command/CommandAllocator.h"
#include "Vertix/Graphics/Command/CommandList.h"
#include "Vertix/Graphics/Command/CommandQueue.h"
#include "Vertix/Windowing/SwapChain.h"

Vertix::GraphicsDevice::GraphicsDevice(const GraphicsDeviceOptions &options) : initialOptions(options) {
    uint32_t dxgiFactoryFlags = 0;

#if defined(_DEBUG) && defined(VERTIX_D3D12_ENABLE_DEBUG_LAYER)
    EnableDebugLayer(dxgiFactoryFlags);
#endif

    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    Microsoft::WRL::ComPtr<D3D12Interface::DXGIAdapter> adapter;
    ThrowIfFailed(EnumerateAdapters(adapter));
    ThrowIfFailed(CreateDevice(adapter));

    ThrowIfFailed(featureSupport.Init(device.Get()));
}

Vertix::GraphicsDevice::~GraphicsDevice() {
#if defined(_DEBUG) && defined(VERTIX_D3D12_ENABLE_DEBUG_LAYER)
    ReportLiveObjects();
#endif
}

std::unique_ptr<Vertix::CommandQueue> Vertix::GraphicsDevice::CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC &queueDesc) const {
    return std::make_unique<CommandQueue>(device.Get(), queueDesc);
}

std::unique_ptr<Vertix::CommandAllocator> Vertix::GraphicsDevice::CreateCommandAllocator(const D3D12_COMMAND_LIST_TYPE type) const {
    return std::make_unique<CommandAllocator>(device.Get(), type);
}

std::unique_ptr<Vertix::CommandList> Vertix::GraphicsDevice::CreateCommandList(const D3D12_COMMAND_LIST_TYPE type) const {
    return std::make_unique<CommandList>(device.Get(), type);
}

std::unique_ptr<Vertix::SwapChain> Vertix::GraphicsDevice::CreateSwapChain(
    const CommandQueue*          commandQueue,
    const HWND                   hWnd,
    const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc) const {
    Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgiSwapChain;
    ThrowIfFailed(
        dxgiFactory->CreateSwapChainForHwnd(
            commandQueue->GetD3D12CommandQueue(),
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &dxgiSwapChain
        )
    );

    Microsoft::WRL::ComPtr<D3D12Interface::DXGISwapChain> swapChain;
    ThrowIfFailed(dxgiSwapChain.As(&dxgiSwapChain));
    return std::make_unique<SwapChain>(device.Get(), commandQueue->GetD3D12CommandQueue(), swapChain);
}

HRESULT Vertix::GraphicsDevice::EnumerateAdapters(Microsoft::WRL::ComPtr<D3D12Interface::DXGIAdapter> &adaptor) const {
    uint32_t adapterIndex = 0;
    auto     enumerator   = [&] {
        return SUCCEEDED(
            dxgiFactory->EnumAdapterByGpuPreference(
                adapterIndex++,
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                IID_PPV_ARGS(&adaptor))
        );
    };

    if (initialOptions.forceSoftwareAdapter) {
        while (enumerator()) {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
            if (const HRESULT hresult = adaptor->GetDesc1(&dxgiAdapterDesc); !SUCCEEDED(hresult)) return hresult;
            if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) break;
        }
        return S_OK;
    }

    while (enumerator()) {
        DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
        if (const HRESULT hresult = adaptor->GetDesc1(&dxgiAdapterDesc); !SUCCEEDED(hresult)) return hresult;
        if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
        return S_OK;
    }

    return E_FAIL;
}

HRESULT Vertix::GraphicsDevice::CreateDevice(const Microsoft::WRL::ComPtr<D3D12Interface::DXGIAdapter> &adaptor) {
    constexpr D3D_FEATURE_LEVEL d3D12FeatureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
    };

    for (const auto &featureLevel : d3D12FeatureLevels) {
        if (featureLevel < initialOptions.minimumFeatureLevel) break;
        if (SUCCEEDED(D3D12CreateDevice(adaptor.Get(), featureLevel, IID_PPV_ARGS(&device)))) {
#if defined(_DEBUG) && defined(VERTIX_D3D12_ENABLE_DEBUG_LAYER)
            ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&debugDevice)));
#endif
            return S_OK;
        }
    }
    return E_FAIL;
}

#if defined(_DEBUG) && defined(VERTIX_D3D12_ENABLE_DEBUG_LAYER)
void Vertix::GraphicsDevice::EnableDebugLayer(uint32_t &dxgiFactoryFlags) {
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
    ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)));
    debug->EnableDebugLayer();

#if VERTIX_DXGI_DEBUG_VERSION >= 1
    dxgiDebug->EnableLeakTrackingForThread();
#endif

    OutputDebugStringW(L"[Vertix] ===== D3D12 Debug Layer Enabled =====\n");
}

void Vertix::GraphicsDevice::ReportLiveObjects() {
    OutputDebugStringW(L"[Vertix] ===== D3D12 Live Device Objects =====\n");
    ThrowIfFailed(debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_FLAGS(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL)));

    dxgiFactory.Reset();

    OutputDebugStringW(L"[Vertix] ===== DXGI Live Objects =====\n");
    ThrowIfFailed(dxgiDebug->ReportLiveObjects(DXGI_DEBUG_DXGI, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)));
}
#endif
