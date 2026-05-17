//
// Created by Natsurainko on 2026/3/17.
//

#ifndef VERTIX_TEXTUREPOOL_HPP
#define VERTIX_TEXTUREPOOL_HPP

#include <d3d12/d3dx12_root_signature.h>

#include "Vertix/Exceptions/HResultException.h"
#include "Vertix/Mixin/IDescriptorAllocator.h"
#include "Vertix/Pool/ResourcePool.hpp"
#include "Vertix/Primitive/Texture.h"

namespace Vertix {
    class TexturePool : public ResourcePool<Texture, TextureHandle> {
    public:
        explicit TexturePool(std::unique_ptr<IDescriptorAllocator> inAllocator) {
            startSlot  = inAllocator->GetStartSlot();
            capacity   = inAllocator->GetTotalCount();
            assert(inAllocator->GetFreeCount() == capacity && "Allocator must be completely empty.");

            allocator  = std::move(inAllocator);
            slotOffset = startSlot;
            if (startSlot == 0) {
                nullHandle = allocator->AllocDescriptorHandle();
                ++slotOffset;
                --capacity;
            }

            slots          = std::make_unique<std::unique_ptr<Texture>[]>(capacity);
            readyCallbacks = std::make_unique<std::vector<std::function<void(const TextureHandle&)>>[]>(capacity);
        }

        explicit TexturePool(
            std::unique_ptr<IDescriptorAllocator> allocator,
            const uint32_t capacity)
        : ResourcePool(capacity), allocator(std::move(allocator))
        {
            assert(allocator->GetFreeCount() == allocator->GetTotalCount() && "Allocator must be completely empty.");
            assert(allocator->GetTotalCount() >= capacity && "Allocator capacity is smaller than pool capacity.");
            startSlot = allocator->GetStartSlot();

            if (startSlot == 0) {
                assert(allocator->GetTotalCount() >= capacity + 1 && "Allocator capacity is smaller than pool capacity + 1, to reserve the null handle.");
                nullHandle = allocator->AllocDescriptorHandle();
            }

            slotOffset = startSlot + (startSlot == 0 ? 1 : 0);
        }

        ~TexturePool() override {
            if (startSlot == 0) {
                allocator->FreeDescriptorHandle(nullHandle);
            }
        }

        [[nodiscard]]
        TextureHandle Allocate(std::unique_ptr<Texture> resource = nullptr) noexcept override {
            assert(!allocator->IsFull() && "ResourcePool capacity exceeded");

            const TextureHandle handle = allocator->AllocDescriptorHandle();
            if (resource) {
                slots[HandleToIndex(handle)] = std::move(resource);
            }

            return handle;
        }

        [[nodiscard]]
        TextureHandle AllocateNamed(
            const std::wstring &name,
            std::unique_ptr<Texture> resource = nullptr) noexcept
        {
            const TextureHandle handle = Allocate(std::move(resource));
            NameResource(handle, name);
            return handle;
        }

        void Free(const TextureHandle &handle) noexcept override {
            namedResources.erase(handle);
            allocator->FreeDescriptorHandle(handle);
        }

        void NameResource(const TextureHandle &handle, const std::wstring &name) noexcept {
            namedResources[handle] = name;
            namedResourceHandles[name] = handle;
        }

        [[nodiscard]]
        bool ContainsNamedResource(const std::wstring &name) const noexcept {
            return namedResourceHandles.contains(name);
        }

        [[nodiscard]]
        TextureHandle GetNamedHandle(const std::wstring &name) noexcept {
            return namedResourceHandles[name];
        }

        [[nodiscard]]
        const std::wstring& GetHandleName(const TextureHandle &handle) noexcept {
            return namedResources[handle];
        }

        [[nodiscard]]
        D3D12_RESOURCE_DESC GetResourceDesc(const TextureHandle &handle) const noexcept {
            const auto* texture = this->GetAs<Texture>(handle);
            return texture->GetResource()->GetDesc();
        }

    protected:
        uint32_t HandleToIndex(const TextureHandle& handle) const noexcept override {
            return handle.slot - slotOffset;
        }

    private:
        std::unique_ptr<IDescriptorAllocator> allocator;
        uint32_t startSlot;
        uint32_t slotOffset;
        TextureHandle nullHandle;

        std::unordered_map<TextureHandle, std::wstring> namedResources{};
        std::unordered_map<std::wstring, TextureHandle> namedResourceHandles{};
    };
}

#endif //VERTIX_TEXTUREPOOL_HPP
