//
// Created by Natsurainko on 2026/3/23.
//

#ifndef VERTIX_FORMATSHELPER_H
#define VERTIX_FORMATSHELPER_H

#include <d3d12/d3d12.h>
#include <d3d12/dxgiformat.h>
#include <wrl/client.h>

namespace Vertix {
    inline bool IsBGRFormat(const DXGI_FORMAT format) noexcept {
        switch (format) {
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8X8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                return true;
            default:
                return false;
        }
    }

    inline bool IsSRGBFormat(const DXGI_FORMAT format) noexcept {
        switch (format) {
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                return true;
            default:
                return false;
        }
    }

    inline bool IsUAVCompatibleFormat(
        const Microsoft::WRL::ComPtr<ID3D12Device> &device,
        const bool typedUAVLoadAdditionalFormats,
        const DXGI_FORMAT format) noexcept
    {
        switch (format) {
            // Unconditionally supported.
            case DXGI_FORMAT_R32_FLOAT:
            case DXGI_FORMAT_R32_UINT:
            case DXGI_FORMAT_R32_SINT:
                return true;

            // All these are supported if this optional feature is set.
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
            case DXGI_FORMAT_R32G32B32A32_UINT:
            case DXGI_FORMAT_R32G32B32A32_SINT:
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
            case DXGI_FORMAT_R16G16B16A16_UINT:
            case DXGI_FORMAT_R16G16B16A16_SINT:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_SINT:
            case DXGI_FORMAT_R16_FLOAT:
            case DXGI_FORMAT_R16_UINT:
            case DXGI_FORMAT_R16_SINT:
            case DXGI_FORMAT_R8_UNORM:
            case DXGI_FORMAT_R8_UINT:
            case DXGI_FORMAT_R8_SINT:
                return typedUAVLoadAdditionalFormats;

            // Conditionally supported by specific devices.
            case DXGI_FORMAT_R16G16B16A16_UNORM:
            case DXGI_FORMAT_R16G16B16A16_SNORM:
            case DXGI_FORMAT_R32G32_FLOAT:
            case DXGI_FORMAT_R32G32_UINT:
            case DXGI_FORMAT_R32G32_SINT:
            case DXGI_FORMAT_R10G10B10A2_UNORM:
            case DXGI_FORMAT_R10G10B10A2_UINT:
            case DXGI_FORMAT_R11G11B10_FLOAT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R16G16_FLOAT:
            case DXGI_FORMAT_R16G16_UNORM:
            case DXGI_FORMAT_R16G16_UINT:
            case DXGI_FORMAT_R16G16_SNORM:
            case DXGI_FORMAT_R16G16_SINT:
            case DXGI_FORMAT_R8G8_UNORM:
            case DXGI_FORMAT_R8G8_UINT:
            case DXGI_FORMAT_R8G8_SNORM:
            case DXGI_FORMAT_R8G8_SINT:
            case DXGI_FORMAT_R16_UNORM:
            case DXGI_FORMAT_R16_SNORM:
            case DXGI_FORMAT_R8_SNORM:
            case DXGI_FORMAT_A8_UNORM:
            case DXGI_FORMAT_B5G6R5_UNORM:
            case DXGI_FORMAT_B5G5R5A1_UNORM:
            case DXGI_FORMAT_B4G4R4A4_UNORM:
                if (typedUAVLoadAdditionalFormats) {
                    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = { format, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
                    if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport)))) {
                        constexpr DWORD mask = D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD | D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE;
                        return (formatSupport.Support2 & mask) == mask;
                    }
                }
                return false;

            default:
                return false;
        }
    }

    inline DXGI_FORMAT ConvertToTypelessFormat(const DXGI_FORMAT format) noexcept {
        switch (format) {
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
            case DXGI_FORMAT_R32G32B32A32_UINT:
            case DXGI_FORMAT_R32G32B32A32_SINT:
                return DXGI_FORMAT_R32G32B32A32_TYPELESS;

            case DXGI_FORMAT_R16G16B16A16_FLOAT:
            case DXGI_FORMAT_R16G16B16A16_UNORM:
            case DXGI_FORMAT_R16G16B16A16_UINT:
            case DXGI_FORMAT_R16G16B16A16_SNORM:
            case DXGI_FORMAT_R16G16B16A16_SINT:
                return DXGI_FORMAT_R16G16B16A16_TYPELESS;

            case DXGI_FORMAT_R32G32_FLOAT:
            case DXGI_FORMAT_R32G32_UINT:
            case DXGI_FORMAT_R32G32_SINT:
                return DXGI_FORMAT_R32G32_TYPELESS;

            case DXGI_FORMAT_R10G10B10A2_UNORM:
            case DXGI_FORMAT_R10G10B10A2_UINT:
                return DXGI_FORMAT_R10G10B10A2_TYPELESS;

            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R8G8B8A8_SINT:
                return DXGI_FORMAT_R8G8B8A8_TYPELESS;

            case DXGI_FORMAT_R16G16_FLOAT:
            case DXGI_FORMAT_R16G16_UNORM:
            case DXGI_FORMAT_R16G16_UINT:
            case DXGI_FORMAT_R16G16_SNORM:
            case DXGI_FORMAT_R16G16_SINT:
                return DXGI_FORMAT_R16G16_TYPELESS;

            case DXGI_FORMAT_R32_FLOAT:
            case DXGI_FORMAT_R32_UINT:
            case DXGI_FORMAT_R32_SINT:
                return DXGI_FORMAT_R32_TYPELESS;

            case DXGI_FORMAT_R8G8_UNORM:
            case DXGI_FORMAT_R8G8_UINT:
            case DXGI_FORMAT_R8G8_SNORM:
            case DXGI_FORMAT_R8G8_SINT:
                return DXGI_FORMAT_R8G8_TYPELESS;

            case DXGI_FORMAT_R16_FLOAT:
            case DXGI_FORMAT_R16_UNORM:
            case DXGI_FORMAT_R16_UINT:
            case DXGI_FORMAT_R16_SNORM:
            case DXGI_FORMAT_R16_SINT:
                return DXGI_FORMAT_R16_TYPELESS;

            case DXGI_FORMAT_R8_UNORM:
            case DXGI_FORMAT_R8_UINT:
            case DXGI_FORMAT_R8_SNORM:
            case DXGI_FORMAT_R8_SINT:
                return DXGI_FORMAT_R8_TYPELESS;

            default:
                return format;
        }
    }
}

#endif //VERTIX_FORMATSHELPER_H