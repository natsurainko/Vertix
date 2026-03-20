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
            for (uint32_t i = Capacity; i-- > 0; )
                freeSlots[freeTop++] = i;
        }

        virtual ~ResourcePool() = default;

        ResourcePool(const ResourcePool&) = delete;
        ResourcePool& operator=(const ResourcePool&) = delete;

        [[nodiscard]]
        virtual THandle Allocate(std::unique_ptr<TResource> resource = nullptr) {
            assert(freeTop > 0 && "ResourcePool capacity exceeded");
            const uint32_t index = freeSlots[--freeTop];

            if (resource) {
                slots[index] = std::move(resource);
            }

            return THandle{index + 1};
        }

        virtual void Fulfill(
            const THandle handle,
            std::unique_ptr<TResource> resource)
        {
            const uint32_t index = handle.slot - 1;
            assert(handle);
            assert(!slots[index] && "Already fulfilled");
            assert(resource);
            slots[index] = std::move(resource);
        }

        virtual void Free(const THandle handle) {
            const uint32_t index = handle.slot - 1;
            assert(handle);
            assert(slots[index] && "Double free detected");
            slots[index].reset();
            freeSlots[freeTop++] = index;
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
            return capacity - freeTop;
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
        uint32_t capacity;

        std::array<std::unique_ptr<TResource>, Capacity> slots {nullptr};
        std::array<uint32_t, Capacity> freeSlots{};
        uint32_t freeTop = 0;
    };
}

#endif //VERTIX_RESOURCEPOOL_HPP