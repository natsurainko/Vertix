//
// Created by Natsurainko on 2026/2/18.
//

#ifndef VERTIX_TEXTURE_H
#define VERTIX_TEXTURE_H

#include <string>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_root_signature.h>
#include <DirectXTK12/WICTextureLoader.h>
#include <DirectXTK12/ResourceUploadBatch.h>
#include <wrl/client.h>

#include "Graphics/DescriptorHeap.h"
#include "Graphics/GraphicsCommandList.h"
#include "Graphics/ResourceUploadHeap.h"

namespace Vertix {
    class Texture {
    public:
        Texture(const Microsoft::WRL::ComPtr<ID3D12Device10> &d3d12Device,
                const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
                DescriptorHeap &srvDescriptorHeap);
        virtual ~Texture();

        [[nodiscard]]
        const Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() const {
            return d3d12Resource;
        }

        [[nodiscard]]
        UINT GetIndexOfDescriptor() const {
            return descriptorHeapIndex;
        }

    protected:
        Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;

        UINT descriptorHeapIndex;
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle{};
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle{};
    };

    class Texture2D : public Texture {
    public:
        Texture2D(const Microsoft::WRL::ComPtr<ID3D12Device10> &d3d12Device,
                 const Microsoft::WRL::ComPtr<ID3D12Resource> &d3d12Resource,
                 DescriptorHeap &srvDescriptorHeap);

        static Texture2D* CreatePixelColorTexture(const float color[4],
                                                  const GraphicsDevice *graphicsDevice,
                                                  const GraphicsCommandList *graphicsCommandList,
                                                  ResourceUploadHeap &resourceUploadHeap,
                                                  DescriptorHeap &srvDescriptorHeap);

        static Texture2D* CreateFromDdsFile(const std::wstring &filename,
                                            const GraphicsDevice *graphicsDevice,
                                            const GraphicsCommandList *graphicsCommandList,
                                            ResourceUploadHeap &resourceUploadHeap,
                                            DescriptorHeap &srvDescriptorHeap);

        static Texture2D* CreateFromFileUsingWIC(const std::wstring &filename,
                                                 const GraphicsDevice *graphicsDevice,
                                                 const GraphicsCommandList *graphicsCommandList,
                                                 ResourceUploadHeap &resourceUploadHeap,
                                                 DescriptorHeap &srvDescriptorHeap,
                                                 DirectX::WIC_LOADER_FLAGS wicLoaderFlags = DirectX::WIC_LOADER_FLAGS::WIC_LOADER_DEFAULT);

        static Texture2D* CreateFromFileUsingWIC(const std::wstring &filename,
                                                 const GraphicsDevice *graphicsDevice,
                                                 DirectX::ResourceUploadBatch &resourceUploadBatch,
                                                 DescriptorHeap &srvDescriptorHeap,
                                                 DirectX::WIC_LOADER_FLAGS wicLoaderFlags = DirectX::WIC_LOADER_FLAGS::WIC_LOADER_DEFAULT);
    };
}

#endif //VERTIX_TEXTURE_H