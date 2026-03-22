//
// Created by Natsurainko on 2026/3/15.
//

#ifndef VERTIX_MATERIALPOOL_H
#define VERTIX_MATERIALPOOL_H

#include <array>
#include <assert.h>
#include <memory>

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
        MaterialHandle Allocate(std::unique_ptr<Material> resource = nullptr) noexcept override {
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
            std::unique_ptr<Material> resource) noexcept override
        {
            ResourcePool<Material, MaterialHandle, Capacity>::Fulfill(handle, std::move(resource));
            MarkDirty(handle);
        }

        void Free(const MaterialHandle handle) noexcept override {
            ResourcePool<Material, MaterialHandle, Capacity>::Free(handle);
            MarkDirty(handle);
        }

        void MarkDirty(const MaterialHandle handle) noexcept {
            dirtySlots[handle.slot - 1] = true;
            needDirtyFlush = true;
        }

        void FlushDirty() {
            if (!needDirtyFlush) return;

            for (uint32_t i = 0; i < Capacity; ++i) {
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
        bool needDirtyFlush = false;
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