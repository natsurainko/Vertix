//
// Created by Natsurainko on 2026/3/17.
//

#ifndef VERTIX_TEXTURELOADER_H
#define VERTIX_TEXTURELOADER_H

#include <string>
#include <DirectXTK12/WICTextureLoader.h>
#include <Vertix/Dispatching/DispatcherQueue.hpp>
#include <Vertix/Graphics/GraphicsDevice.h>
#include <Vertix/Graphics/ResourceUploadHeap.hpp>
#include <Vertix/Pool/TexturePool.hpp>
#include <Vertix/Primitive/Texture.h>

#include "Vertix.Engine/VERTIX_ENGINE_EXPORT.h"

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

    class TextureAsyncLoader {
        struct TextureLoadRequest {
            TextureHandle             Handle;
            std::wstring              FilePath;
            DirectX::WIC_LOADER_FLAGS WicLoaderFlags;
        };

        struct TextureLoadingContext {
            TextureHandle Handle;
            Texture*      TexturePtr = nullptr;
        };

    public:
        TextureAsyncLoader(
            TexturePool* texturePool,
            GraphicsDevice* graphicsDevice,
            const Microsoft::WRL::ComPtr<ID3D12CommandQueue> &copyCommandQueue,
            const Microsoft::WRL::ComPtr<ID3D12CommandQueue> &computeCommandQueue = nullptr,
            const bool autoGenerateTextureMipmaps = true)
            : texturePool(texturePool), graphicsDevice(graphicsDevice),
              autoGenerateTextureMipmaps(autoGenerateTextureMipmaps),
              copyCommandQueue(copyCommandQueue), computeCommandQueue(computeCommandQueue) {
            d3d12Device = graphicsDevice->GetD3D12Device();
        }

        VERTIX_ENGINE_API TextureHandle LoadTextureAsync(
            const std::wstring &filePath,
            const std::wstring* resourceName = nullptr,
            DirectX::WIC_LOADER_FLAGS wicLoaderFlags = DirectX::WIC_LOADER_DEFAULT,
            std::function<void(const TextureHandle&)> textureLoadedCallback = nullptr);

        VERTIX_ENGINE_API void ExecuteAsync(DispatcherQueue* dispatcherQueue);

    private:
        TexturePool* texturePool;
        GraphicsDevice* graphicsDevice;

        std::vector<TextureLoadRequest> textureLoadRequests;
        bool autoGenerateTextureMipmaps;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> copyCommandQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> computeCommandQueue;

        Microsoft::WRL::ComPtr<ID3D12Device10> d3d12Device;
    };
}

#endif //VERTIX_TEXTURELOADER_H
