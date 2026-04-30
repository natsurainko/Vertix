//
// Created by Natsurainko on 2026/3/22.
//

#ifndef VERTIX_DYNAMICRESOURCEPOOL_H
#define VERTIX_DYNAMICRESOURCEPOOL_H

#include <cassert>
#include <functional>
#include <memory>
#include <set>

namespace Vertix {
    template<typename TResource, typename THandle>
    class DynamicResourcePool {
    public:
        DynamicResourcePool() noexcept = default;
        virtual ~DynamicResourcePool() noexcept = default;

        DynamicResourcePool(const DynamicResourcePool&) = delete;
        DynamicResourcePool& operator=(const DynamicResourcePool&) = delete;

        [[nodiscard]]
        virtual THandle Allocate(std::unique_ptr<TResource> resource = nullptr) {
            THandle handle;

            if (!returnedHandles.empty()) {
                auto iterator = returnedHandles.begin();
                handle = *iterator;
                returnedHandles.erase(iterator);
            } else {
                ++allocatedMaxHandle.slot;
                handle = allocatedMaxHandle;
            }

            if (resource) {
                slots[handle] = std::move(resource);
            }

            return handle;
        }

        virtual void Fulfill(
            const THandle handle,
            std::unique_ptr<TResource> resource)
        {
            assert(handle);
            assert(!slots.contains(handle) || !slots.at(handle) && "Already fulfilled");
            assert(resource);
            slots[handle] = std::move(resource);
            NotifyReady(handle);
        }

        virtual void Free(const THandle handle) {
            assert(handle);
            assert(slots.contains(handle) && slots.at(handle) && "Double free detected");
            slots[handle].reset();

            if (handle == allocatedMaxHandle) {
                --allocatedMaxHandle.slot;

                while (allocatedMaxHandle && returnedHandles.contains(allocatedMaxHandle)) {
                    returnedHandles.erase(allocatedMaxHandle);
                    --allocatedMaxHandle.slot;
                }
            } else {
                returnedHandles.insert(handle);
            }
        }

        void NotifyReady(const THandle handle) noexcept {
            auto &callbacks = readyCallbacks[handle];
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
            readyCallbacks[handle].emplace_back(std::move(textureLoadedCallback));
        }

        [[nodiscard]]
        bool IsReady(const THandle handle) const noexcept {
            if (!handle) return false;
            auto it = slots.find(handle);
            return it != slots.end() && it->second != nullptr;
        }

        [[nodiscard]]
        TResource* Get(const THandle handle) noexcept {
            if (!handle) return nullptr;
            return slots[handle].get();
        }

        template<class T>
        T* GetAs(const THandle handle) noexcept {
            return static_cast<T*>(Get(handle));
        }

        [[nodiscard]]
        uint32_t GetCount() const noexcept {
            return allocatedMaxHandle.slot - static_cast<uint32_t>(returnedHandles.size());
        }

        [[nodiscard]]
        bool IsEmpty() const noexcept {
            return GetCount() == 0;
        }

    protected:
        std::unordered_map<THandle, std::unique_ptr<TResource>> slots;
        std::unordered_map<THandle, std::vector<std::function<void(THandle)>>> readyCallbacks{};

        std::set<THandle> returnedHandles;
        THandle allocatedMaxHandle{};
    };
}

#endif //VERTIX_DYNAMICRESOURCEPOOL_H