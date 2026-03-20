//
// Created by Natsurainko on 2026/3/15.
//

#ifndef VERTIX_MATERIALPOOL_H
#define VERTIX_MATERIALPOOL_H

#include <array>
#include <cassert>
#include <memory>
#include <mutex>

#include "ResourcePool.hpp"
#include "Graphics/Buffers/StructuredBuffer.hpp"
#include "Mixin/IFillConstants.h"
#include "Primitive/Material.h"

namespace Vertix {
    template<typename TConstants, uint32_t Capacity = 1024>
    class MaterialPool : public ResourcePool<Material, MaterialHandle, Capacity> {
    public:
        explicit MaterialPool(const GraphicsDevice* graphicsDevice) : constantBuffer(graphicsDevice, Capacity) {}
        ~MaterialPool() override = default;

        [[nodiscard]]
        MaterialHandle Allocate(std::unique_ptr<Material> resource = nullptr) override {
            assert(this->freeTop > 0 && "ResourcePool capacity exceeded");
            const uint32_t index = this->freeSlots[--this->freeTop];
            const MaterialHandle handle{index + 1};

            if (resource) {
                this->slots[index] = std::move(resource);
                MarkDirty(handle);
            }

            return handle;
        }

        void Fulfill(
            MaterialHandle handle,
            std::unique_ptr<Material> resource) override
        {
            ResourcePool<Material, MaterialHandle, Capacity>::Fulfill(handle, std::move(resource));
            MarkDirty(handle);
        }

        void Free(const MaterialHandle handle) override {
            ResourcePool<Material, MaterialHandle, Capacity>::Free(handle);
            MarkDirty(handle);
        }

        void MarkDirty(const MaterialHandle handle) {
            std::lock_guard lock(dirtyMutex);
            dirtySlots[handle.slot - 1] = true;
            needDirtyFlush.store(true, std::memory_order_release);
        }

        void FlushDirty() {
            if (!needDirtyFlush.load(std::memory_order_acquire))
                return;

            std::array<bool, Capacity> toFlush{};
            {
                std::lock_guard lock(dirtyMutex);
                std::swap(toFlush, dirtySlots);
                needDirtyFlush.store(false, std::memory_order_relaxed);
            }

            for (uint32_t i = 0; i < Capacity; ++i) {
                if (!toFlush[i]) continue;

                TConstants constants{};
                if (this->slots[i]) {
                    FillConstants(*this->slots[i], constants);
                }
                constantBuffer.FillAt(i, constants);
            }
        }

        [[nodiscard]]
        D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const noexcept {
            return constantBuffer.GetGpuVirtualAddress();
        }

    protected:
        virtual void FillConstants(
            const Material& material,
            TConstants& out) const
        {
            const auto* fillable = dynamic_cast<const IFillConstants<TConstants>*>(&material);
            assert(fillable);
            fillable->Fill(out);
        }

    private:
        std::atomic<bool> needDirtyFlush = false;
        std::mutex dirtyMutex;
        std::array<bool, Capacity> dirtySlots{};

        StructuredBuffer<TConstants> constantBuffer;
    };

    template<uint32_t Capacity = 1024>
    class DefaultMaterialPool : public MaterialPool<DefaultMaterialConstants, Capacity> {
    public:
        explicit DefaultMaterialPool(const GraphicsDevice* graphicsDevice) : MaterialPool<DefaultMaterialConstants, Capacity>(graphicsDevice) {}
    };
}

#endif //VERTIX_MATERIALPOOL_H