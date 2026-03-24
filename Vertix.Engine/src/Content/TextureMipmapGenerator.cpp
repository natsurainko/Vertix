//
// Created by Natsurainko on 2026/3/23.
//

#include "Content/TextureMipmapGenerator.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>
#include <d3d12/d3dx12_root_signature.h>

#include "GenerateMipmap.h"
#include "Exceptions/HResultException.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ResourceUploadHeap.hpp"
#include "Helpers/FormatsHelper.h"

Vertix::Engine::TextureMipmapGenerator::TextureMipmapGenerator(const GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {
    d3d12Device = graphicsDevice->GetD3D12Device();
    computeRootSignature = CreateComputeRootSignature(d3d12Device);
    computePipelineState = CreateComputePipelineState(d3d12Device, computeRootSignature);
}

void Vertix::Engine::TextureMipmapGenerator::Generate(
    const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &computeCommandList,
    ResourceUploadHeap &resourceUploadHeap,
    const D3D12_RESOURCE_STATES beforeState,
    const D3D12_RESOURCE_STATES afterState) const
{
    const auto resourceDesc = d3d12Resource->GetDesc();
    const auto resourceFormat = resourceDesc.Format;

    // No Mipmap Levels need to be generated.
    if (resourceDesc.MipLevels <= 1) return;
    // Only supports generating single-layer Texture2D mipmaps.
    if (resourceDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || resourceDesc.DepthOrArraySize != 1) return;

    const bool typedUAVLoadAdditionalFormatsSupported = graphicsDevice->IsTypedUAVLoadAdditionalFormatsSupported();
    const bool isUAVCompatibleFormat = IsUAVCompatibleFormat(d3d12Device, typedUAVLoadAdditionalFormatsSupported, resourceFormat);
    const bool isSRGBFormat = IsSRGBFormat(resourceFormat);
    const bool isBGRFormat = IsBGRFormat(resourceFormat);

    // The Texture2D Format does not support generating Mipmaps.
    if (!(isUAVCompatibleFormat || isSRGBFormat || isBGRFormat)) return;

    Microsoft::WRL::ComPtr<ID3D12Resource> uavResource;

    if (isUAVCompatibleFormat) {
        if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0) {
            D3D12_RESOURCE_BARRIER barriers[] {
                CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), beforeState, D3D12_RESOURCE_STATE_COPY_SOURCE),
                CD3DX12_RESOURCE_BARRIER::Transition(nullptr, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
                CD3DX12_RESOURCE_BARRIER::Transition(nullptr, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE),
                CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
                CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, afterState),
            };

            computeCommandList->ResourceBarrier(1, &barriers[0]);
            CopyToUAVCompatibleResource(d3d12Resource, computeCommandList, resourceDesc, uavResource, ConvertToTypelessFormat(resourceDesc.Format));
            barriers[1].Transition.pResource = uavResource.Get();
            barriers[2].Transition.pResource = uavResource.Get();

            computeCommandList->ResourceBarrier(1, &barriers[1]);
            ProcessUAVCompatibleResource(uavResource, computeCommandList, resourceDesc.Format, resourceUploadHeap);
            computeCommandList->ResourceBarrier(2, &barriers[2]);
            computeCommandList->CopyResource(d3d12Resource.Get(), uavResource.Get());
            computeCommandList->ResourceBarrier(1, &barriers[4]);
        } else {
            const D3D12_RESOURCE_BARRIER barriers[] {
                CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), beforeState, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
                CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, afterState),
            };

            computeCommandList->ResourceBarrier(1, &barriers[0]);
            ProcessUAVCompatibleResource(d3d12Resource, computeCommandList, resourceDesc.Format, resourceUploadHeap);
            computeCommandList->ResourceBarrier(1, &barriers[1]);
        }
    } else if (!graphicsDevice->IsTypedUAVLoadAdditionalFormatsSupported()) {
        assert(false && "Needs TypedUAVLoadAdditionalFormats device support for sRGB/BGR");
        return;
    } else if (isBGRFormat) {
        Microsoft::WRL::ComPtr<ID3D12Resource> aliasResource;
        D3D12_RESOURCE_BARRIER barriers[] {
            CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), beforeState, D3D12_RESOURCE_STATE_COPY_SOURCE),
            CD3DX12_RESOURCE_BARRIER::Aliasing(nullptr, nullptr),
            CD3DX12_RESOURCE_BARRIER::Transition(nullptr, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Aliasing(nullptr, nullptr),
            CD3DX12_RESOURCE_BARRIER::Transition(nullptr, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, afterState),
        };

        computeCommandList->ResourceBarrier(1, &barriers[0]);
        CopyBGRToUAVCompatibleResource(d3d12Resource, computeCommandList, resourceDesc, uavResource, aliasResource, resourceUploadHeap);
        barriers[1].Aliasing.pResourceBefore = aliasResource.Get();
        barriers[1].Aliasing.pResourceAfter = uavResource.Get();
        barriers[2].Transition.pResource = uavResource.Get();
        barriers[3].Aliasing.pResourceBefore = uavResource.Get();
        barriers[3].Aliasing.pResourceAfter = aliasResource.Get();
        barriers[4].Transition.pResource = aliasResource.Get();

        computeCommandList->ResourceBarrier(2, &barriers[1]);
        ProcessUAVCompatibleResource(uavResource, computeCommandList, DXGI_FORMAT_R8G8B8A8_UNORM, resourceUploadHeap);
        computeCommandList->ResourceBarrier(3, &barriers[3]);
        computeCommandList->CopyResource(d3d12Resource.Get(), aliasResource.Get());
        computeCommandList->ResourceBarrier(1, &barriers[6]);
    } else {
        assert(isSRGBFormat && !isBGRFormat);
        D3D12_RESOURCE_BARRIER barriers[] {
            CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), beforeState, D3D12_RESOURCE_STATE_COPY_SOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(nullptr, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(nullptr, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(d3d12Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, afterState),
        };

        computeCommandList->ResourceBarrier(1, &barriers[0]);
        CopyToUAVCompatibleResource(d3d12Resource, computeCommandList, resourceDesc, uavResource, DXGI_FORMAT_R8G8B8A8_UNORM);
        barriers[1].Transition.pResource = uavResource.Get();
        barriers[2].Transition.pResource = uavResource.Get();

        computeCommandList->ResourceBarrier(1, &barriers[1]);
        ProcessUAVCompatibleResource(uavResource, computeCommandList, DXGI_FORMAT_R8G8B8A8_UNORM, resourceUploadHeap);
        computeCommandList->ResourceBarrier(2, &barriers[2]);
        computeCommandList->CopyResource(d3d12Resource.Get(), uavResource.Get());
        computeCommandList->ResourceBarrier(1, &barriers[4]);
    }

    if (uavResource) {
        resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(uavResource));
    }
}

void Vertix::Engine::TextureMipmapGenerator::CopyToUAVCompatibleResource(
    const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &computeCommandList,
    const D3D12_RESOURCE_DESC &resourceDesc,
    Microsoft::WRL::ComPtr<ID3D12Resource> &uavResource,
    const DXGI_FORMAT destResourceFormat) const
{
    D3D12_RESOURCE_DESC stagingDesc = resourceDesc;
    stagingDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    stagingDesc.Format = destResourceFormat;

    const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &stagingDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&uavResource)
    ));

    const CD3DX12_TEXTURE_COPY_LOCATION srcLocation(d3d12Resource.Get(), 0);
    const CD3DX12_TEXTURE_COPY_LOCATION destLocation(uavResource.Get(), 0);
    computeCommandList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);
}

void Vertix::Engine::TextureMipmapGenerator::CopyBGRToUAVCompatibleResource(
    const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &computeCommandList,
    const D3D12_RESOURCE_DESC &resourceDesc,
    Microsoft::WRL::ComPtr<ID3D12Resource> &resourceCopy,
    Microsoft::WRL::ComPtr<ID3D12Resource> &aliasCopy,
    ResourceUploadHeap &resourceUploadHeap) const
{
    D3D12_RESOURCE_DESC copyDesc = resourceDesc;
    copyDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    copyDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    copyDesc.Layout = D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;

    D3D12_RESOURCE_DESC aliasDesc = resourceDesc;
    aliasDesc.Format = (resourceDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM || resourceDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) ? DXGI_FORMAT_B8G8R8X8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;
    aliasDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    aliasDesc.Layout = D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;

    D3D12_HEAP_DESC heapDesc{};
    heapDesc.SizeInBytes = d3d12Device->GetResourceAllocationInfo(0, 1, &copyDesc).SizeInBytes;
    heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
    heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

    Microsoft::WRL::ComPtr<ID3D12Heap> heap;
    ThrowIfFailed(d3d12Device->CreateHeap(&heapDesc, IID_PPV_ARGS(&heap)));
    ThrowIfFailed(d3d12Device->CreatePlacedResource(heap.Get(), 0, &copyDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resourceCopy)));
    ThrowIfFailed(d3d12Device->CreatePlacedResource(heap.Get(), 0, &aliasDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&aliasCopy)));

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Aliasing(nullptr, aliasCopy.Get());
    const CD3DX12_TEXTURE_COPY_LOCATION srcLocation(d3d12Resource.Get(), 0);
    const CD3DX12_TEXTURE_COPY_LOCATION destLocation(aliasCopy.Get(), 0);

    computeCommandList->ResourceBarrier(1, &barrier);
    computeCommandList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);

    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(heap));
}

void Vertix::Engine::TextureMipmapGenerator::ProcessUAVCompatibleResource(
    const Microsoft::WRL::ComPtr<ID3D12Resource> &uavResource,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &computeCommandList,
    const DXGI_FORMAT uavResourceViewFormat,
    ResourceUploadHeap &resourceUploadHeap) const
{
    ID3D12Resource* uavResourcePtr = uavResource.Get();
    const D3D12_RESOURCE_DESC resourceDesc = uavResource->GetDesc();

    auto descriptorHeap = std::make_unique<DescriptorHeap>(graphicsDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, resourceDesc.MipLevels, true);
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
    descriptorHeap->AllocDescriptorHandle(cpuHandle, gpuHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = uavResourceViewFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
    d3d12Device->CreateShaderResourceView(uavResourcePtr, &srvDesc, cpuHandle);

    for (UINT16 i = 1; i < resourceDesc.MipLevels; ++i) {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.Format = uavResourceViewFormat;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = i;

        descriptorHeap->AllocDescriptorHandle(cpuHandle, gpuHandle);
        d3d12Device->CreateUnorderedAccessView(uavResourcePtr, nullptr, &uavDesc, cpuHandle);
    }

    computeCommandList->SetComputeRootSignature(computeRootSignature.Get());
    computeCommandList->SetPipelineState(computePipelineState.Get());
    computeCommandList->SetDescriptorHeaps(1, descriptorHeap->GetDescriptorHeap().GetAddressOf());
    computeCommandList->SetComputeRootDescriptorTable(1, descriptorHeap->GetGpuDescriptorHandleForHeapStart());

    const UINT descriptorLength = descriptorHeap->GetDescriptorLength();
    CD3DX12_GPU_DESCRIPTOR_HANDLE uavHandle(descriptorHeap->GetGpuDescriptorHandleForHeapStart(), static_cast<INT>(descriptorLength));

    const auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(uavResourcePtr);
    D3D12_RESOURCE_BARRIER srvToUavBarrier = CD3DX12_RESOURCE_BARRIER::Transition(uavResourcePtr, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    D3D12_RESOURCE_BARRIER uavToSrvBarrier = CD3DX12_RESOURCE_BARRIER::Transition(uavResourcePtr, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    MipConstants constants{};

    auto mipWidth = static_cast<UINT32>(resourceDesc.Width);
    UINT32 mipHeight = resourceDesc.Height;
    for (UINT32 i = 1; i < resourceDesc.MipLevels; ++i) {
        mipWidth = std::max<UINT32>(1, mipWidth >> 1);
        mipHeight = std::max<UINT32>(1, mipHeight >> 1);

        srvToUavBarrier.Transition.Subresource = i;
        uavToSrvBarrier.Transition.Subresource = i;

        constants.InputMipmapLevel = i - 1;
        constants.OutputMipmapTexelSize = DirectX::SimpleMath::Vector2 { 1 / static_cast<float>(mipWidth), 1 / static_cast<float>(mipHeight) };

        computeCommandList->ResourceBarrier(1, &srvToUavBarrier);
        computeCommandList->SetComputeRootDescriptorTable(2, uavHandle);
        computeCommandList->SetComputeRoot32BitConstants(0, 3, &constants, 0);
        computeCommandList->Dispatch((mipWidth + 7) / 8, (mipHeight + 7) / 8, 1);
        computeCommandList->ResourceBarrier(1, &uavBarrier);
        computeCommandList->ResourceBarrier(1, &uavToSrvBarrier);

        uavHandle.Offset(static_cast<INT>(descriptorLength));
    }

    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(std::move(descriptorHeap)));
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> Vertix::Engine::TextureMipmapGenerator::CreateComputeRootSignature(const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device) {
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    constexpr D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_DESCRIPTOR_RANGE descRanges[2];
    descRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    descRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    CD3DX12_STATIC_SAMPLER_DESC pointSampler(0);
    pointSampler.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    pointSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    pointSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    pointSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

    CD3DX12_ROOT_PARAMETER rootParameters[3];
    rootParameters[0].InitAsConstants(3, 0);
    rootParameters[1].InitAsDescriptorTable(1, &descRanges[0]);
    rootParameters[2].InitAsDescriptorTable(1, &descRanges[1]);

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters = 3;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.NumStaticSamplers = 1;
    rootSignatureDesc.pStaticSamplers = &pointSampler;
    rootSignatureDesc.Flags = rootSignatureFlags;

    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;

    ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    ThrowIfFailed(d3d12Device->CreateRootSignature(0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));

    return rootSignature;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> Vertix::Engine::TextureMipmapGenerator::CreateComputePipelineState(
    const Microsoft::WRL::ComPtr<ID3D12Device> &d3d12Device,
    const Microsoft::WRL::ComPtr<ID3D12RootSignature> &rootSignature)
{
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.CS.pShaderBytecode = SHADER_BYTECODE_GENERATE_MIPMAP;
    psoDesc.CS.BytecodeLength = sizeof(SHADER_BYTECODE_GENERATE_MIPMAP);
    psoDesc.pRootSignature = rootSignature.Get();
    ThrowIfFailed(d3d12Device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));

    return pipelineState;
}
