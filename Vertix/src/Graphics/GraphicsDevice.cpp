//
// Created by Natsurainko on 2025/12/23.
//

#include "Graphics/GraphicsDevice.h"

#include <dxgidebug.h>

#include "Exceptions/HResultException.h"
#include "Graphics/SwapChain.h"

using Microsoft::WRL::ComPtr;

constexpr D3D_FEATURE_LEVEL D3D12_Feature_Levels[] = {
    D3D_FEATURE_LEVEL_12_2,
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
};

Vertix::GraphicsDevice::GraphicsDevice(const bool useSoftware): useSoftwareRendering(useSoftware) {
    UINT dxgiFactoryFlags = 0;

#ifndef NDEBUG
#ifndef NO_D3D12_DEBUG_LAYER
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebug)))) {
        d3dDebug->EnableDebugLayer();

        // d3dDebug->SetEnableGPUBasedValidation(TRUE);
        // d3dDebug->SetEnableSynchronizedCommandQueueValidation(TRUE);

        // debugController->SetEnableAutoName(TRUE);

        // D3D12_DEBUG_FEATURE debugFeatures = D3D12_DEBUG_FEATURE_NONE;
        // debugFeatures |= D3D12_DEBUG_FEATURE_ALLOW_BEHAVIOR_CHANGING_DEBUG_AIDS;
        // debugFeatures |= D3D12_DEBUG_FEATURE_CONSERVATIVE_RESOURCE_STATE_TRACKING;
        // debugFeatures |= D3D12_DEBUG_FEATURE_DISABLE_VIRTUALIZED_BUNDLES_VALIDATION;

        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

        // d3dDebug->SetEnableAutoName(FALSE);
        OutputDebugStringW(L"D3D12 Debug Layer Enabled (ID3D12Debug5)\n");
    }
#endif
#endif

    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));
    ThrowIfFailed(CreateSuitableDevice());

    SUCCEEDED(d3d12Device->CheckFeatureSupport(
        D3D12_FEATURE_D3D12_OPTIONS,
        &d3d12FeatureOptions,
        sizeof(d3d12FeatureOptions)));

    constexpr D3D12_COMMAND_QUEUE_DESC queueDesc {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
    };

    ThrowIfFailed(d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3d12CommandQueue)));
}

void Vertix::GraphicsDevice::WaitForGPU() const {
    ComPtr<ID3D12Fence> fence;
    ThrowIfFailed(d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    const HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent) {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    ThrowIfFailed(d3d12CommandQueue->Signal(fence.Get(), 1));
    ThrowIfFailed(fence->SetEventOnCompletion(1, fenceEvent));

    WaitForSingleObject(fenceEvent, INFINITE);
    CloseHandle(fenceEvent);
}

Vertix::GraphicsDevice::~GraphicsDevice() {
    WaitForGPU();
}

void Vertix::GraphicsDevice::CreateCommandQueue(
    ComPtr<ID3D12CommandQueue> &commandQueue,
    const D3D12_COMMAND_QUEUE_DESC& queueDesc) const
{
    ThrowIfFailed(d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
}

HRESULT Vertix::GraphicsDevice::CreateSuitableDevice() {
    ComPtr<IDXGIAdapter1> dxgiAdapter = nullptr;
    if (!useSoftwareRendering) {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(dxgiFactory->EnumAdapterByGpuPreference(
                adapterIndex,
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                IID_PPV_ARGS(&dxgiAdapter)));
            ++adapterIndex) {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
            ThrowIfFailed(dxgiAdapter->GetDesc1(&dxgiAdapterDesc));

            if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue;
            }

            break;
        }
    }

    if (!dxgiAdapter) {
        ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter)));
    }

    for (const auto & featureLevel : D3D12_Feature_Levels) {
        if (SUCCEEDED(D3D12CreateDevice(
            dxgiAdapter.Get(),
            featureLevel,
            IID_PPV_ARGS(&d3d12Device)))) {
            return S_OK;
        }
    }

    return E_FAIL;
}
