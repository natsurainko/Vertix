//
// Created by Natsurainko on 2026/3/15.
//

#ifndef VERTIX_MATERIALPOOL_H
#define VERTIX_MATERIALPOOL_H

#include <cassert>
#include <memory>

#include "Vertix/Graphics/Buffers/StructuredBuffer.hpp"
#include "Vertix/Mixin/IFillConstants.h"
#include "Vertix/Pool/ResourcePool.hpp"
#include "Vertix/Primitive/Material.h"

namespace Vertix {
    template<typename TConstants>
    class MaterialPool : public ResourcePool<Material, MaterialHandle> {
    public:
        explicit MaterialPool(
            const GraphicsDevice* graphicsDevice,
            uint32_t capacity)
        : ResourcePool(capacity), capacity(capacity), constantBuffer(graphicsDevice, capacity)
        {
            dirtySlots = std::make_unique<bool[]>(capacity);
            nullHandle = MaterialPool::Allocate();
        }

        ~MaterialPool() override = default;

        [[nodiscard]]
        MaterialHandle Allocate(std::unique_ptr<Material> resource = nullptr) noexcept override {
            assert(this->freeTop > 0 && "ResourcePool capacity exceeded");
            const uint32_t index = this->freeSlots[--this->freeTop];
            const MaterialHandle handle{index};

            if (resource) {
                this->slots[index] = std::move(resource);
                MarkDirty(handle);
            }

            return handle;
        }

        void Fulfill(
            const MaterialHandle &handle,
            std::unique_ptr<Material> resource) noexcept override
        {
            ResourcePool::Fulfill(handle, std::move(resource));
            MarkDirty(handle);
        }

        void Free(const MaterialHandle &handle) noexcept override {
            ResourcePool::Free(handle);
            MarkDirty(handle);
        }

        void MarkDirty(const MaterialHandle &handle) noexcept {
            dirtySlots[handle.slot] = true;
            needDirtyFlush = true;
        }

        void FlushDirty() {
            if (!needDirtyFlush) return;

            for (uint32_t i = 0; i < capacity; ++i) {
                if (!dirtySlots[i]) continue;

                TConstants constants{};
                if (this->slots[i]) {
                    FillConstants(*this->slots[i], constants);
                }
                constantBuffer.FillAt(i, constants);
                dirtySlots[i] = false;
            }
        }

        [[nodiscard]]
        D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const noexcept { return constantBuffer.GetGpuVirtualAddress(); }

    protected:
        virtual void FillConstants(
            const Material& material,
            TConstants& out) const
        {
            const auto* fillable = dynamic_cast<const IFillConstants<TConstants>*>(&material);
            assert(fillable);
            fillable->Fill(out);
        }

        uint32_t HandleToIndex(const ResourceHandle<MaterialTag> &handle) const noexcept override { return handle.slot; }

    private:
        uint32_t capacity;
        MaterialHandle nullHandle;
        bool needDirtyFlush = false;

        std::unique_ptr<bool[]> dirtySlots;

        StructuredBuffer<TConstants> constantBuffer;
    };
}

#endif //VERTIX_MATERIALPOOL_H
