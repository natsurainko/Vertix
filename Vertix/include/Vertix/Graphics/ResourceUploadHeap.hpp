//
// Created by Natsurainko on 2026/2/22.
//

#pragma once

#include <memory>
#include <vector>
#include <wrl/client.h>

#include "Vertix/Mixin/IResourceUploading.h"

namespace Vertix {
    class ResourceUploadHeap {
        std::vector<std::unique_ptr<IResourceUploading>> resources;

    public:
        void                 Store(std::unique_ptr<IResourceUploading> resource) noexcept { resources.push_back(std::move(resource)); }
        void                 Clear() noexcept { resources.clear(); }
        [[nodiscard]] size_t Size() const noexcept { return resources.size(); }

        template <typename T>
        std::unique_ptr<IResourceUploading> CreateUploadResource(std::unique_ptr<T> ptr) {
            struct TResourceUploading : IResourceUploading {
                explicit           TResourceUploading(std::unique_ptr<T> ptr) : ptr(std::move(ptr)) {}
                std::unique_ptr<T> ptr;
            };
            return std::make_unique<TResourceUploading>(std::move(ptr));
        }

        template <typename T>
        std::unique_ptr<IResourceUploading> CreateUploadResource(const Microsoft::WRL::ComPtr<T> &ptr) {
            struct TResourceUploading : IResourceUploading {
                explicit                  TResourceUploading(const Microsoft::WRL::ComPtr<T> &ptr) : ptr(ptr) {}
                Microsoft::WRL::ComPtr<T> ptr;
            };
            return std::make_unique<TResourceUploading>(ptr);
        }

        template <std::invocable<ID3D12Resource*> Func>
        void CommitUploadResource(
            ID3D12Device*               device,
            const D3D12_RESOURCE_DESC & resourceDesc,
            const D3D12_RESOURCE_STATES initialStates,
            Func &&                     func) {
            Microsoft::WRL::ComPtr<ID3D12Resource> resource;

            const auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            ThrowIfFailed(
                device->CreateCommittedResource(
                    &uploadHeap,
                    D3D12_HEAP_FLAG_NONE,
                    &resourceDesc,
                    initialStates,
                    nullptr,
                    IID_PPV_ARGS(&resource)
                )
            );

            std::invoke(std::forward<Func>(func), resource.Get());
            struct TResourceUploading : IResourceUploading {
                explicit                               TResourceUploading(const Microsoft::WRL::ComPtr<ID3D12Resource> &ptr) : ptr(ptr) {}
                Microsoft::WRL::ComPtr<ID3D12Resource> ptr;
            };
            Store(std::make_unique<TResourceUploading>(std::move(resource)));
        }
    };
}
