//
// Created by Natsurainko on 2026/2/20.
//

#include "Rendering/Texture.h"

#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>
#include <d3d12/d3dx12_resource_helpers.h>
#include <DirectXTK12/DDSTextureLoader.h>
#include <DirectXTK12/WICTextureLoader.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsCommandList.h"

Vertix::Texture::Texture(const Microsoft::WRL::ComPtr<ID3D12Device10> &d3d12Device,
                         const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
                         DescriptorHeap &srvDescriptorHeap): d3d12Resource(d3d12Resource) {
    if (srvDescriptorHeap.IsFull())
        throw std::exception("The descriptor heap is full.");

    srvDescriptorHeap.AllocDescriptorHandle(cpuDescriptorHandle, gpuDescriptorHandle, &descriptorHeapIndex);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    D3D12_RESOURCE_DESC resourceDesc = d3d12Resource->GetDesc();
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = resourceDesc.Format;

    switch (resourceDesc.Dimension) {
        case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            srvDesc.Texture1D.MostDetailedMip = 0;
            srvDesc.Texture1D.MipLevels = resourceDesc.MipLevels;
            break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
            break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            srvDesc.Texture3D.MostDetailedMip = 0;
            srvDesc.Texture3D.MipLevels = resourceDesc.MipLevels;
            break;
        default:
            throw std::exception("Invalid D3D12_RESOURCE_DIMENSION");
    }

    d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), &srvDesc, cpuDescriptorHandle);
}

Vertix::Texture::~Texture() = default;

Vertix::Texture2D::Texture2D(const Microsoft::WRL::ComPtr<ID3D12Device10> &d3d12Device,
                             const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
                             DescriptorHeap &srvDescriptorHeap) : Texture(d3d12Device, d3d12Resource, srvDescriptorHeap) {
}

Vertix::Texture2D* Vertix::Texture2D::CreatePixelColorTexture(const float color[4],
                                                              const GraphicsDevice* graphicsDevice,
                                                              const GraphicsCommandList* graphicsCommandList,
                                                              ResourceUploadHeap &resourceUploadHeap,
                                                              DescriptorHeap &srvDescriptorHeap) {
    const auto &device = graphicsDevice->GetD3D12Device();
    const auto &commandList = graphicsCommandList->GetD3D12GraphicsCommandList();
    constexpr UINT64 textureSize = sizeof(float) * 4;

    Microsoft::WRL::ComPtr<ID3D12Resource> tempUploadResource, d3d12Resource;

    {
        const CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(textureSize);
        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &uploadResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&tempUploadResource)
        ));

        void* mappedPointer = nullptr;
        ThrowIfFailed(tempUploadResource->Map(0, nullptr, &mappedPointer));
        memcpy(mappedPointer, color, textureSize);
        tempUploadResource->Unmap(0, nullptr);
    }

    {
        const CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
        const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1);
        ThrowIfFailed(device->CreateCommittedResource(
            &defaultHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&d3d12Resource)
        ));

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
        device->GetCopyableFootprints(&resourceDesc, 0, 1, 0, &layout, nullptr, nullptr, nullptr);

        const CD3DX12_TEXTURE_COPY_LOCATION destLocation(d3d12Resource.Get(), 0);
        const CD3DX12_TEXTURE_COPY_LOCATION srcLocation(tempUploadResource.Get(), layout);

        commandList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);
    }

    {
        const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        commandList->ResourceBarrier(1, &barrier);
    }

    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(tempUploadResource));
    return new Texture2D(device, d3d12Resource, srvDescriptorHeap);
}

Vertix::Texture2D* Vertix::Texture2D::CreateFromDdsFile(const std::wstring &filename,
                                                        const GraphicsDevice* graphicsDevice,
                                                        const GraphicsCommandList* graphicsCommandList,
                                                        ResourceUploadHeap &resourceUploadHeap,
                                                        DescriptorHeap &srvDescriptorHeap) {
    const auto &device = graphicsDevice->GetD3D12Device();
    const auto &commandList = graphicsCommandList->GetD3D12GraphicsCommandList();

    Microsoft::WRL::ComPtr<ID3D12Resource> tempUploadResource, d3d12Resource;
    std::unique_ptr<uint8_t[]> ddsData;
    std::vector<D3D12_SUBRESOURCE_DATA> subresourceDatas;

    ThrowIfFailed(DirectX::LoadDDSTextureFromFile(
        device.Get(),
        filename.c_str(),
        &d3d12Resource,
        ddsData,
        subresourceDatas
    ));

    {
        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(
            d3d12Resource.Get(), 0, static_cast<UINT>(subresourceDatas.size())
        );

        const CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &uploadResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&tempUploadResource)
        ));
    }

    {
        UpdateSubresources(
            commandList.Get(),
            d3d12Resource.Get(),
            tempUploadResource.Get(),
            0, 0,
            static_cast<UINT>(subresourceDatas.size()),
            subresourceDatas.data()
        );

        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        commandList->ResourceBarrier(1, &barrier);
    }

    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(tempUploadResource));
    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(std::move(ddsData)));
    return new Texture2D(device, d3d12Resource, srvDescriptorHeap);
}

Vertix::Texture2D* Vertix::Texture2D::CreateFromFileUsingWIC(const std::wstring &filename,
                                                              const GraphicsDevice *graphicsDevice,
                                                              const GraphicsCommandList *graphicsCommandList,
                                                              ResourceUploadHeap &resourceUploadHeap,
                                                              DescriptorHeap &srvDescriptorHeap,
                                                              const DirectX::WIC_LOADER_FLAGS wicLoaderFlags) {
    const auto &device = graphicsDevice->GetD3D12Device();
    const auto &commandList = graphicsCommandList->GetD3D12GraphicsCommandList();

    Microsoft::WRL::ComPtr<ID3D12Resource> tempUploadResource, d3d12Resource;
    std::unique_ptr<uint8_t[]> imageData;
    D3D12_SUBRESOURCE_DATA subresourceData;

    ThrowIfFailed(DirectX::LoadWICTextureFromFileEx(
        device.Get(),
        filename.c_str(),
        0,
        D3D12_RESOURCE_FLAG_NONE,
        wicLoaderFlags,
        &d3d12Resource,
        imageData,
        subresourceData
    ));

    {
        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(d3d12Resource.Get(), 0, 1);
        const CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &uploadResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&tempUploadResource)
        ));
    }

    {
        UpdateSubresources(
            commandList.Get(),
            d3d12Resource.Get(),
            tempUploadResource.Get(),
            0, 0,
            1,
            &subresourceData
        );

        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            d3d12Resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        commandList->ResourceBarrier(1, &barrier);
    }

    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(tempUploadResource));
    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(std::move(imageData)));
    return new Texture2D(device, d3d12Resource, srvDescriptorHeap);
}

Vertix::Texture2D* Vertix::Texture2D::CreateFromFileUsingWIC(const std::wstring &filename,
                                                             const GraphicsDevice *graphicsDevice,
                                                             DirectX::ResourceUploadBatch &resourceUploadBatch,
                                                             DescriptorHeap &srvDescriptorHeap,
                                                             const DirectX::WIC_LOADER_FLAGS wicLoaderFlags) {
    const auto &device = graphicsDevice->GetD3D12Device();

    Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
    ThrowIfFailed(DirectX::CreateWICTextureFromFileEx(
        device.Get(),
        resourceUploadBatch,
        filename.c_str(),
        0,
        D3D12_RESOURCE_FLAG_NONE,
        wicLoaderFlags,
        &d3d12Resource
    ));

    return new Texture2D(device, d3d12Resource, srvDescriptorHeap);
}