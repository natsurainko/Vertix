//
// Created by Natsurainko on 2026/5/17.
//

#pragma once

#include <set>

#include "IDescriptorAllocator.h"

namespace Vertix {
    class DescriptorRange : public IDescriptorAllocator {
        friend class DescriptorHeap;

        uint32_t           startSlot;
        uint32_t           slotCount;
        uint32_t           descriptorLength;
        bool               shaderVisible;
        std::set<uint32_t> freeSlots;

        DescriptorHeap* parentHeap;

    public:
        VERTIX_API explicit DescriptorRange(
            DescriptorHeap* parentHeap,
            uint32_t        startSlot,
            uint32_t        slotCount);

        VERTIX_API ~DescriptorRange() override;

        VERTIX_API DescriptorHandle AllocDescriptorHandle() override;
        VERTIX_API void             AllocDescriptorHandles(uint32_t count, DescriptorHandle* handles);

        VERTIX_API void FreeDescriptorHandle(const DescriptorHandle &handle) override;
        VERTIX_API void FreeDescriptorHandles(uint32_t count, const DescriptorHandle* handles);

        [[nodiscard]] bool     IsFull() const noexcept override { return freeSlots.empty(); }
        [[nodiscard]] uint32_t GetFreeCount() const noexcept override { return static_cast<uint32_t>(freeSlots.size()); }
        [[nodiscard]] uint32_t GetTotalCount() const noexcept override { return slotCount; }
        [[nodiscard]] uint32_t GetStartSlot() const noexcept override { return startSlot; }
    };
}
