//
// Created by Natsurainko on 2026/3/17.
//

#include "Content/TextureLoader.h"

#include <d3dx12_core.h>
#include <d3dx12_resource_helpers.h>
#include <DDSTextureLoader.h>

#include "Exceptions/HResultException.h"
#include "Graphics/GraphicsDevice.h"

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

Vertix::TextureHandle Vertix::Engine::TextureAsyncLoader::LoadTextureAsync(
    const std::wstring &filePath,
    DirectX::WIC_LOADER_FLAGS wicLoaderFlags,
    std::function<void(TextureHandle)> textureLoadedCallback)
{
    if (texturePool->ContainsNamedResource(filePath))
        return texturePool->GetNamedHandle(filePath);

    const TextureHandle handle = texturePool->Allocate();
    texturePool->NameResource(handle, filePath);
    textureLoadRequests.emplace_back(handle, filePath, wicLoaderFlags, std::move(textureLoadedCallback));
    return handle;
}

void Vertix::Engine::TextureAsyncLoader::ExecuteAsync(DispatcherQueue* dispatcherQueue) {
    std::thread([
        requests   = std::move(textureLoadRequests),
        device     = d3d12Device,
        copyQueue  = copyCommandQueue,
        pool       = texturePool,
        dispatcherQueue
    ]() mutable -> void
    {
        ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

        ResourceUploadHeap resourceUploadHeap{};
        GraphicsCommandList copyCommandList(device, copyQueue, D3D12_COMMAND_LIST_TYPE_COPY);
        std::unordered_map<TextureHandle, std::function<void(TextureHandle)>> callbacks;

        copyCommandList.BeginCommand(nullptr);
        {
            for (const auto &[handle, filePath, flags, callback] : requests) {
                Texture* texture;
                if (filePath.ends_with(L".dds")) {
                    texture = TextureLoader::CreateFromDdsFile(
                        filePath, device,
                        copyCommandList.GetD3D12GraphicsCommandList(),
                        resourceUploadHeap);
                } else {
                    texture = TextureLoader::CreateFromFileUsingWIC(
                        filePath, device,
                        copyCommandList.GetD3D12GraphicsCommandList(),
                        resourceUploadHeap, flags);
                }

                pool->Fulfill(handle, std::unique_ptr<Texture>(texture));

                if (callback)
                    callbacks.emplace(handle, callback);
            }
        }
        copyCommandList.EndCommand();
        copyCommandList.WaitForCommand();

        dispatcherQueue->Enqueue([ callbacks = std::move(callbacks), pool ] {
            for (auto &[handle, callback] : callbacks) {
                CreateSRVAndBarrier(pool, handle);
                callback(handle);
            }
        });

        CoUninitialize();
    }).detach();
}

void Vertix::Engine::TextureAsyncLoader::CreateSRVAndBarrier(TexturePool<>* texturePool, TextureHandle handle) {
    auto* texture = texturePool->GetAs<Texture>(handle);
    const D3D12_RESOURCE_DESC desc = texture->GetResource()->GetDesc();

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

    texturePool->CreateShaderResourceView(handle, srvDesc);
    texturePool->MarkBarrier(handle);
}
