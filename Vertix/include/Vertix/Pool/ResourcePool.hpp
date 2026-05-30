//
// Created by Natsurainko on 2026/3/15.
//

#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>

namespace Vertix {
    // We agree that all ResourcePool or DynamicResourcePool and their derived types are not thread-safe.
    // Therefore, the best practice in multithreading is to call their functions on the main thread via DispatcherQueue::Enqueue.
    template <typename TResource, typename THandle>
    class ResourcePool {
    public:
        explicit ResourcePool(const std::uint32_t capacity) noexcept : capacity(capacity) {
            slots          = std::make_unique<std::unique_ptr<TResource>[]>(capacity);
            freeSlots      = std::make_unique<uint32_t[]>(capacity);
            readyCallbacks = std::make_unique<std::vector<std::function<void(const THandle &)>>[]>(capacity);

            for (uint32_t i = capacity; i-- > 0;)
                freeSlots[freeTop++] = i;
        }

        virtual ~ResourcePool() noexcept = default;

        ResourcePool(const ResourcePool &)            = delete;
        ResourcePool& operator=(const ResourcePool &) = delete;

        [[nodiscard]]
        virtual THandle Allocate(std::unique_ptr<TResource> resource = nullptr) noexcept {
            assert(freeTop > 0 && "ResourcePool capacity exceeded");
            const uint32_t index = freeSlots[--freeTop];

            if (resource) {
                slots[index] = std::move(resource);
            }

            return THandle { index + 1 };
        }

        virtual void Fulfill(
            const THandle &            handle,
            std::unique_ptr<TResource> resource) noexcept {
            const uint32_t index = HandleToIndex(handle);
            assert(handle);
            assert(!slots[index] && "Already fulfilled");
            assert(resource);
            slots[index] = std::move(resource);
            NotifyReady(handle);
        }

        virtual void Free(const THandle &handle) noexcept {
            const uint32_t index = HandleToIndex(handle);
            assert(handle);
            assert(slots[index] && "Double free detected");
            slots[index].reset();
            freeSlots[freeTop++] = index;
        }

        void NotifyReady(const THandle &handle) noexcept {
            auto &callbacks = readyCallbacks[HandleToIndex(handle)];
            for (auto &callback : callbacks) {
                if (callback) {
                    callback(handle);
                }
            }
            callbacks.clear();
        }

        void OnReady(const THandle &handle, std::function<void(const THandle &)> textureLoadedCallback) noexcept {
            if (IsReady(handle)) {
                textureLoadedCallback(handle);
                return;
            }
            readyCallbacks[HandleToIndex(handle)].emplace_back(std::move(textureLoadedCallback));
        }

        [[nodiscard]]
        TResource* Get(const THandle &handle) const noexcept {
            if (!handle) return nullptr;
            return slots[HandleToIndex(handle)].get();
        }

        template <class T>
        T* GetAs(const THandle &handle) const noexcept {
            return static_cast<T*>(Get(handle));
        }

        [[nodiscard]] virtual uint32_t GetCount() const noexcept { return capacity - freeTop; }
        [[nodiscard]] virtual uint32_t GetFree() const noexcept { return freeTop; }
        [[nodiscard]] virtual uint32_t GetCapacity() const noexcept { return capacity; }
        [[nodiscard]] virtual bool     IsFull() const noexcept { return !freeTop; }
        [[nodiscard]] virtual bool     IsEmpty() const noexcept { return freeTop == capacity; }
        [[nodiscard]] bool             IsReady(const THandle &handle) const noexcept { return handle && slots[HandleToIndex(handle)]; }

    protected:
        ResourcePool() noexcept = default;

        uint32_t capacity;
        uint32_t freeTop = 0;

        std::unique_ptr<std::unique_ptr<TResource>[]> slots;
        std::unique_ptr<uint32_t[]>                   freeSlots;

        std::unique_ptr<std::vector<std::function<void(const THandle &)>>[]> readyCallbacks;

        virtual uint32_t HandleToIndex(const THandle &handle) const noexcept {
            return handle.slot - 1;
        }
    };
}
