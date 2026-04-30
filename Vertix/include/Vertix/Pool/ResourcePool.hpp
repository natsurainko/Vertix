//
// Created by Natsurainko on 2026/3/15.
//

#ifndef VERTIX_RESOURCEPOOL_HPP
#define VERTIX_RESOURCEPOOL_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>

namespace Vertix {
    // We agree that all ResourcePool or DynamicResourcePool and their derived types are not thread-safe.
    // Therefore, the best practice in multithreading is to call their functions on the main thread via DispatcherQueue::Enqueue.
    template<typename TResource, typename THandle, std::uint32_t Capacity>
    class ResourcePool {
    public:
        ResourcePool() noexcept {
            for (uint32_t i = Capacity; i-- > 0; )
                freeSlots[freeTop++] = i;
        }

        virtual ~ResourcePool() noexcept = default;

        ResourcePool(const ResourcePool&) = delete;
        ResourcePool& operator=(const ResourcePool&) = delete;

        [[nodiscard]]
        virtual THandle Allocate(std::unique_ptr<TResource> resource = nullptr) noexcept {
            assert(freeTop > 0 && "ResourcePool capacity exceeded");
            const uint32_t index = freeSlots[--freeTop];

            if (resource) {
                slots[index] = std::move(resource);
            }

            return THandle{index + 1};
        }

        virtual void Fulfill(
            const THandle handle,
            std::unique_ptr<TResource> resource) noexcept
        {
            const uint32_t index = handle.slot - 1;
            assert(handle);
            assert(!slots[index] && "Already fulfilled");
            assert(resource);
            slots[index] = std::move(resource);
            NotifyReady(handle);
        }

        virtual void Free(const THandle handle) noexcept {
            const uint32_t index = handle.slot - 1;
            assert(handle);
            assert(slots[index] && "Double free detected");
            slots[index].reset();
            freeSlots[freeTop++] = index;
        }

        void NotifyReady(const THandle handle) noexcept {
            auto &callbacks = readyCallbacks[handle.slot - 1];
            for (auto& callback : callbacks) {
                if (callback) {
                    callback(handle);
                }
            }
            callbacks.clear();
        }

        void OnReady(const THandle handle, std::function<void(THandle)> textureLoadedCallback) noexcept {
            if (IsReady(handle)) {
                textureLoadedCallback(handle);
                return;
            }
            readyCallbacks[handle.slot - 1].emplace_back(std::move(textureLoadedCallback));
        }

        [[nodiscard]]
        bool IsReady(const THandle handle) const noexcept {
            return handle && slots[handle.slot - 1];
        }

        [[nodiscard]]
        TResource* Get(const THandle handle) noexcept {
            if (!handle) return nullptr;
            return slots[handle.slot - 1].get();
        }

        template<class T>
        T* GetAs(const THandle handle) noexcept {
            return static_cast<T*>(Get(handle));
        }

        [[nodiscard]]
        uint32_t GetCount() const noexcept {
            return Capacity - freeTop;
        }

        [[nodiscard]]
        uint32_t GetFree() const noexcept {
            return freeTop;
        }

        [[nodiscard]]
        static constexpr uint32_t GetCapacity() noexcept {
            return Capacity;
        }

        [[nodiscard]]
        bool IsFull() const noexcept {
            return !freeTop;
        }

        [[nodiscard]]
        bool IsEmpty() const noexcept {
            return freeTop == Capacity;
        }

    protected:
        uint32_t freeTop = 0;

        std::array<std::unique_ptr<TResource>, Capacity> slots {nullptr};
        std::array<uint32_t, Capacity> freeSlots{};
        std::array<std::vector<std::function<void(THandle)>>, Capacity> readyCallbacks{};
    };
}

#endif //VERTIX_RESOURCEPOOL_HPP