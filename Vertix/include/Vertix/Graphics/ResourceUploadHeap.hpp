//
// Created by Natsurainko on 2026/2/22.
//

#ifndef VERTIX_RESOURCEUPLOADHEAP_H
#define VERTIX_RESOURCEUPLOADHEAP_H

#include <memory>
#include <vector>
#include <wrl/client.h>

#include "Vertix/Mixin/IResourceUploading.h"

namespace Vertix {
    class ResourceUploadHeap {
    public:
        void Store(std::unique_ptr<IResourceUploading> resource) noexcept {
            resources.push_back(std::move(resource));
        }

        void Clear() {
            resources.clear();
        }

        [[nodiscard]]
        size_t Size() const noexcept {
            return resources.size();
        }

        template <typename T>
        std::unique_ptr<IResourceUploading> CreateUploadResource(std::unique_ptr<T> ptr) {
            struct TResourceUploading : IResourceUploading {
                explicit TResourceUploading(std::unique_ptr<T> ptr) : ptr(std::move(ptr)) {}
                std::unique_ptr<T> ptr;
            };
            return std::make_unique<TResourceUploading>(std::move(ptr));
        }

        template <typename T>
        std::unique_ptr<IResourceUploading> CreateUploadResource(const Microsoft::WRL::ComPtr<T> &ptr) {
            struct TResourceUploading : IResourceUploading {
                explicit TResourceUploading(const Microsoft::WRL::ComPtr<T> &ptr) : ptr(ptr) {}
                Microsoft::WRL::ComPtr<T> ptr;
            };
            return std::make_unique<TResourceUploading>(ptr);
        }
    private:
        std::vector<std::unique_ptr<IResourceUploading>> resources;
    };
}

#endif //VERTIX_RESOURCEUPLOADHEAP_H
