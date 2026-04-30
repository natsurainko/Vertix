//
// Created by Natsurainko on 2026/3/17.
//

#include "Vertix.Engine/Content/TextureLoader.h"

#include <filesystem>
#include <d3d12/d3dx12_core.h>
#include <d3d12/d3dx12_resource_helpers.h>
#include <DirectXTK12/DDSTextureLoader.h>
#include <Vertix/Exceptions/HResultException.h>
#include <Vertix/Graphics/GraphicsDevice.h>

#include "Vertix.Engine/Content/TextureMipmapGenerator.h"

Vertix::Texture2D* Vertix::Engine::TextureLoader::CreatePixelColorTexture(
    const float color[4],
    const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5>& commandList,
    ResourceUploadHeap &resourceUploadHeap)
{
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

    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(tempUploadResource));
    return new Texture2D(d3d12Resource);
}

Vertix::Texture2D* Vertix::Engine::TextureLoader::CreateFromDdsFile(
    const std::wstring &filename,
    const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5>& commandList,
    ResourceUploadHeap &resourceUploadHeap)
{
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

    UpdateSubresources(
        commandList.Get(),
        d3d12Resource.Get(),
        tempUploadResource.Get(),
        0, 0,
        static_cast<UINT>(subresourceDatas.size()),
        subresourceDatas.data()
    );

    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(tempUploadResource));
    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(std::move(ddsData)));
    return new Texture2D(d3d12Resource);
}

Vertix::Texture2D* Vertix::Engine::TextureLoader::CreateFromFileUsingWIC(
    const std::wstring &filename,
    const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5>& commandList,
    ResourceUploadHeap &resourceUploadHeap,
    const DirectX::WIC_LOADER_FLAGS wicLoaderFlags)
{
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

    UpdateSubresources(
        commandList.Get(),
        d3d12Resource.Get(),
        tempUploadResource.Get(),
        0, 0,
        1,
        &subresourceData
    );

    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(tempUploadResource));
    resourceUploadHeap.Store(resourceUploadHeap.CreateUploadResource(std::move(imageData)));
    return new Texture2D(d3d12Resource);
}

Vertix::Texture2D* Vertix::Engine::TextureLoader::CreateFromFileUsingWIC(
    const std::wstring &filename,
    const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
    DirectX::ResourceUploadBatch &resourceUploadBatch,
    const DirectX::WIC_LOADER_FLAGS wicLoaderFlags)
{
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

    return new Texture2D(d3d12Resource);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CreateShaderResourceViewDesc(const Microsoft::WRL::ComPtr<ID3D12Resource> &texture) {
    const D3D12_RESOURCE_DESC desc = texture->GetDesc();
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = desc.Format;

    switch (desc.Dimension) {
        case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            srvDesc.ViewDimension         = D3D12_SRV_DIMENSION_TEXTURE1D;
            srvDesc.Texture1D.MipLevels   = desc.MipLevels;
            break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            srvDesc.ViewDimension         = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels   = desc.MipLevels;
            break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
            srvDesc.ViewDimension         = D3D12_SRV_DIMENSION_TEXTURE3D;
            srvDesc.Texture3D.MipLevels   = desc.MipLevels;
            break;
        default:
            throw std::runtime_error("Invalid D3D12_RESOURCE_DIMENSION");
    }

    return srvDesc;
}

Vertix::TextureHandle Vertix::Engine::TextureAsyncLoader::LoadTextureAsync(
    const std::wstring &filePath,
    const std::wstring* resourceName,
    DirectX::WIC_LOADER_FLAGS wicLoaderFlags,
    std::function<void(TextureHandle)> textureLoadedCallback)
{
    const std::wstring &handleName = resourceName ? *resourceName : filePath;
    const bool loaded = texturePool->ContainsNamedResource(handleName);
    const TextureHandle handle = loaded
        ? texturePool->GetNamedHandle(handleName)
        : texturePool->AllocateNamed(handleName);

    if (!loaded) {
        textureLoadRequests.emplace_back(
            handle,
            filePath,
            wicLoaderFlags
        );
    }

    if (textureLoadedCallback) {
        texturePool->OnReady(handle, std::move(textureLoadedCallback));
    }

    return handle;
}

void Vertix::Engine::TextureAsyncLoader::ExecuteAsync(DispatcherQueue* dispatcherQueue) {
    std::thread([
        requests       = std::move(textureLoadRequests),
        device         = d3d12Device,
        copyQueue      = copyCommandQueue,
        computeQueue   = computeCommandQueue,
        pool           = texturePool,
        graphicsDevice = graphicsDevice,
        genMipmaps     = autoGenerateTextureMipmaps,
        dispatcherQueue
    ]() mutable -> void
    {
        std::vector<TextureLoadingContext> textureLoadingContexts;

        // Load Textures
        ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
        {
            ResourceUploadHeap resourceUploadHeap{};
            GraphicsCommandList copyCommandList(device, copyQueue, D3D12_COMMAND_LIST_TYPE_COPY);
            copyCommandList.BeginCommand(nullptr);
            {
                for (const auto &[handle, filePath, flags] : requests) {
                    if (!std::filesystem::exists(filePath)) continue;

                    Texture* texture = filePath.ends_with(L".dds")
                        ? TextureLoader::CreateFromDdsFile(
                            filePath, device,
                            copyCommandList.GetD3D12GraphicsCommandList(),
                            resourceUploadHeap)
                        : TextureLoader::CreateFromFileUsingWIC(
                            filePath, device,
                            copyCommandList.GetD3D12GraphicsCommandList(),
                            resourceUploadHeap, flags);

                    textureLoadingContexts.emplace_back(handle, texture);
                    ThrowIfFailed(texture->GetResource()->SetName(pool->GetHandleName(handle).c_str()));
                }
            }
            copyCommandList.EndCommand();
            copyCommandList.WaitForCommand();
        }
        CoUninitialize();

        // Generate Mipmaps
        if (genMipmaps && computeQueue) {
            ResourceUploadHeap resourceUploadHeap{};
            const TextureMipmapGenerator textureMipmapGenerator { graphicsDevice };
            GraphicsCommandList computeCommandList(device, computeQueue, D3D12_COMMAND_LIST_TYPE_COMPUTE);
            computeCommandList.BeginCommand(nullptr);
            {
                for (const auto &[_, texture] : textureLoadingContexts) {
                    textureMipmapGenerator.Generate(
                        texture->GetResource(),
                        computeCommandList.GetD3D12GraphicsCommandList(),
                        resourceUploadHeap,
                        D3D12_RESOURCE_STATE_COMMON,
                        D3D12_RESOURCE_STATE_COMMON);
                }
            }
            computeCommandList.EndCommand();
            computeCommandList.WaitForCommand();
        }

        dispatcherQueue->Enqueue([
            textureFulfills = std::move(textureLoadingContexts),
            pool,
            graphicsDevice
        ] {
            const auto& d3d12Device = graphicsDevice->GetD3D12Device();

            for (const auto &[handle, texture] : textureFulfills) {
                const auto d3d12Resource = texture->GetResource();
                const auto srvDesc = CreateShaderResourceViewDesc(d3d12Resource);
                const auto descriptorHandle = pool->GetDescriptorHandle(handle);

                d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), &srvDesc, descriptorHandle);
                pool->Fulfill(handle, std::unique_ptr<Texture>(texture));
            }
        });
    }).detach();
}
