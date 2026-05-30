//
// Created by Natsurainko on 2026/3/23.
//

#ifndef VERTIX_TEXTUREMIPMAPGENERATOR_H
#define VERTIX_TEXTUREMIPMAPGENERATOR_H

#include <d3d12/d3d12.h>
#include <DirectXTK12/SimpleMath.h>
#include <Vertix/Graphics/Command/GraphicsCommandList.h>
#include <Vertix/Graphics/ResourceUploadHeap.hpp>
#include <wrl/client.h>

#include "Vertix.Engine/VERTIX_ENGINE_EXPORT.h"

namespace Vertix::Engine {
    class TextureMipmapGenerator {
        struct MipConstants {
            DirectX::SimpleMath::Vector2 OutputMipmapTexelSize;
            std::uint32_t                InputMipmapLevel;
            std::uint32_t                IsSRGBFormat;
        };

    public:
        VERTIX_ENGINE_API explicit TextureMipmapGenerator(const GraphicsDevice* graphicsDevice);

        VERTIX_ENGINE_API void Generate(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &computeCommandList,
            ResourceUploadHeap &resourceUploadHeap,
            D3D12_RESOURCE_STATES beforeState,
            D3D12_RESOURCE_STATES afterState) const;

    private:
        const GraphicsDevice* graphicsDevice;

        Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState;
        Microsoft::WRL::ComPtr<ID3D12RootSignature> computeRootSignature;

        void CopyToUAVCompatibleResource(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &computeCommandList,
            const D3D12_RESOURCE_DESC &resourceDesc,
            Microsoft::WRL::ComPtr<ID3D12Resource> &uavResource,
            DXGI_FORMAT destResourceFormat) const;

        void CopyBGRToUAVCompatibleResource(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &computeCommandList,
            const D3D12_RESOURCE_DESC &resourceDesc,
            Microsoft::WRL::ComPtr<ID3D12Resource> &resourceCopy,
            Microsoft::WRL::ComPtr<ID3D12Resource> &aliasCopy,
            ResourceUploadHeap &resourceUploadHeap) const;

        void ProcessUAVCompatibleResource(
            const Microsoft::WRL::ComPtr<ID3D12Resource> &uavResource,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &computeCommandList,
            DXGI_FORMAT uavResourceViewFormat,
            ResourceUploadHeap &resourceUploadHeap,
            bool isSRGBFormat = false) const;

        static Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateComputePipelineState(
            const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
            const Microsoft::WRL::ComPtr<ID3D12RootSignature> &rootSignature);

        static Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateComputeRootSignature(const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device);
    };
}

#endif //VERTIX_TEXTUREMIPMAPGENERATOR_H
