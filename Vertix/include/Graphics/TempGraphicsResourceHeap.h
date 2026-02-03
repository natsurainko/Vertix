//
// Created by Natsurainko on 2026/1/13.
//

#ifndef VERTIX_TEMPGRAPHICSRESOURCEHEAP_H
#define VERTIX_TEMPGRAPHICSRESOURCEHEAP_H

#include <vector>

namespace Vertix {
    template <typename TResource>
    class TempGraphicsResourceHeap {
    public:
        void Store(TResource&& resource) {
            resources.push_back(std::move(resource));
        }

        void Clear() {
            resources.clear();
        }

        [[nodiscard]]
        size_t Size() const {
            return resources.size();
        }

        ~TempGraphicsResourceHeap() {
            Clear();
        }

        TempGraphicsResourceHeap() = default;
        TempGraphicsResourceHeap(const TempGraphicsResourceHeap&) = delete;
        TempGraphicsResourceHeap& operator=(const TempGraphicsResourceHeap&) = delete;
        TempGraphicsResourceHeap(TempGraphicsResourceHeap&& other) noexcept
            : resources(std::move(other.resources)) {}
        TempGraphicsResourceHeap& operator=(TempGraphicsResourceHeap&& other) noexcept {
            if (this != &other) {
                Clear();
                resources = std::move(other.resources);
            }
            return *this;
        }

    private:
        std::vector<TResource> resources;
    };
}

#endif //VERTIX_TEMPGRAPHICSRESOURCEHEAP_H