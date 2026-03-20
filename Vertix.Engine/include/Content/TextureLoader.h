//
// Created by Natsurainko on 2026/3/17.
//

#ifndef VERTIX_TEXTURELOADER_H
#define VERTIX_TEXTURELOADER_H

#include <string>
#include <WICTextureLoader.h>

#include "VERTIX_ENGINE_EXPORT.h"
#include "Dispatching/DispatcherQueue.hpp"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ResourceUploadHeap.hpp"
#include "Pool/TexturePool.hpp"
#include "Primitive/Texture.h"

namespace Vertix::Engine {
    class TextureLoader {
    public:
        static VERTIX_ENGINE_API Texture2D* CreatePixelColorTexture(
            const float color[4],
            const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
            ResourceUploadHeap &resourceUploadHeap);

        static VERTIX_ENGINE_API Texture2D* CreateFromDdsFile(
            const std::wstring &filename,
            const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
            ResourceUploadHeap &resourceUploadHeap);

        static VERTIX_ENGINE_API Texture2D* CreateFromFileUsingWIC(
            const std::wstring &filename,
            const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
            const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> &commandList,
            ResourceUploadHeap &resourceUploadHeap,
            DirectX::WIC_LOADER_FLAGS wicLoaderFlags = DirectX::WIC_LOADER_FLAGS::WIC_LOADER_DEFAULT);

        static VERTIX_ENGINE_API Texture2D* CreateFromFileUsingWIC(
            const std::wstring &filename,
            const Microsoft::WRL::ComPtr<ID3D12Device10> &device,
            DirectX::ResourceUploadBatch &resourceUploadBatch,
            DirectX::WIC_LOADER_FLAGS wicLoaderFlags = DirectX::WIC_LOADER_FLAGS::WIC_LOADER_DEFAULT);
    };

    class VERTIX_ENGINE_API TextureAsyncLoader {
        struct TextureLoadRequest {
            TextureHandle Handle;
            std::wstring FilePath;
            DirectX::WIC_LOADER_FLAGS WicLoaderFlags;
            D3D12_RESOURCE_STATES beforeState;
            D3D12_RESOURCE_STATES afterState;
        };

        struct TextureLoadingContext {
            TextureHandle Handle;
            Texture* TexturePtr;
            D3D12_RESOURCE_STATES beforeState;
            D3D12_RESOURCE_STATES afterState;
        };

    public:
        TextureAsyncLoader(
            TexturePool<>* texturePool,
            GraphicsDevice* graphicsDevice,
            const Microsoft::WRL::ComPtr<ID3D12CommandQueue> &copyCommandQueue)
            : texturePool(texturePool), graphicsDevice(graphicsDevice), copyCommandQueue(copyCommandQueue), d3d12Device(graphicsDevice->GetD3D12Device()) {}

        TextureHandle LoadTextureAsync(
            const std::wstring &filePath,
            const std::wstring* resourceName = nullptr,
            DirectX::WIC_LOADER_FLAGS wicLoaderFlags = DirectX::WIC_LOADER_DEFAULT,
            D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATES afterState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            std::function<void(TextureHandle)> textureLoadedCallback = nullptr);

        void ExecuteAsync(
            DispatcherQueue* dispatcherQueue,
            GraphicsCommandList* directCommandList);

    private:
        TexturePool<>* texturePool;
        GraphicsDevice* graphicsDevice;

        std::vector<TextureLoadRequest> textureLoadRequests;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> copyCommandQueue;
        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
    };
}

#endif //VERTIX_TEXTURELOADER_H