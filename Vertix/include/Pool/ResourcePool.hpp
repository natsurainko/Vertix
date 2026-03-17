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
    template<typename TResource, typename THandle, std::uint32_t Capacity = 1024>
    class ResourcePool {
    public:
        ResourcePool() : capacity(Capacity) {
            for (uint32_t i = Capacity - 1; i-- > 1; )
                freeSlots[freeTop++] = i;
        }

        virtual ~ResourcePool() = default;

        ResourcePool(const ResourcePool&) = delete;
        ResourcePool& operator=(const ResourcePool&) = delete;

        [[nodiscard]]
        virtual THandle Allocate(std::unique_ptr<TResource> resource = nullptr) {
            assert(freeTop > 0 && "ResourcePool capacity exceeded");
            const uint32_t slot = freeSlots[--freeTop];

            if (resource) {
                slots[slot] = std::move(resource);
            }

            return THandle{slot};
        }

        virtual void Fulfill(THandle handle, std::unique_ptr<TResource> resource) {
            assert(handle);
            assert(!slots[handle.slot] && "Already fulfilled");
            assert(resource);
            slots[handle.slot] = std::move(resource);
        }

        virtual void Free(const THandle handle) {
            assert(handle);
            slots[handle.slot].reset();
            freeSlots[freeTop++] = handle.slot;
        }

        [[nodiscard]]
        bool IsReady(THandle handle) const noexcept {
            return handle && slots[handle.slot];
        }

        [[nodiscard]]
        TResource* Get(THandle handle) noexcept {
            if (!handle) return nullptr;
            return slots[handle.slot].get();
        }

        template<class T>
        T* GetAs(const THandle handle) noexcept {
            return static_cast<T*>(Get(handle));
        }

        [[nodiscard]]
        uint32_t GetCount() const noexcept {
            return capacity - 1 - freeTop;
        }

        [[nodiscard]]
        uint32_t GetFree() const noexcept {
            return freeTop;
        }

        [[nodiscard]]
        uint32_t GetCapacity() const noexcept {
            return capacity - 1;
        }

        [[nodiscard]]
        bool IsFull() const noexcept {
            return !freeTop;
        }

        [[nodiscard]]
        bool IsEmpty() const noexcept {
            return freeTop == Capacity - 1;
        }

    protected:
        uint32_t capacity;

        std::array<std::unique_ptr<TResource>, Capacity> slots {nullptr};
        std::array<uint32_t, Capacity> freeSlots{};
        uint32_t freeTop = 0;
    };
}

#endif //VERTIX_RESOURCEPOOL_HPP