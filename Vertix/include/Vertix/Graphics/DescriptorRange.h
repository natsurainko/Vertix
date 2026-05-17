//
// Created by Natsurainko on 2026/5/17.
//

#ifndef VERTIX_DESCRIPTORRANGE_H
#define VERTIX_DESCRIPTORRANGE_H

#include <set>

#include "Vertix/VERTIX_EXPORT.h"
#include "Vertix/Mixin/IDescriptorAllocator.h"

namespace Vertix {
    class DescriptorHeap;

    class DescriptorRange : public IDescriptorAllocator {
    public:
        VERTIX_API explicit DescriptorRange(
            DescriptorHeap* parentHeap,
            uint32_t startSlot,
            uint32_t slotCount);

        VERTIX_API ~DescriptorRange() override;

        VERTIX_API DescriptorHeapHandle AllocDescriptorHandle() override;
        VERTIX_API void AllocDescriptorHandles(uint32_t count, DescriptorHeapHandle* handles);

        VERTIX_API void FreeDescriptorHandle(const DescriptorHeapHandle &handle) override;
        VERTIX_API void FreeDescriptorHandles(uint32_t count, const DescriptorHeapHandle* handles);

        [[nodiscard]] bool     IsFull() const noexcept override { return freeSlots.empty(); }
        [[nodiscard]] uint32_t GetFreeCount() const noexcept override { return static_cast<uint32_t>(freeSlots.size()); }
        [[nodiscard]] uint32_t GetTotalCount() const noexcept override { return slotCount; }
        [[nodiscard]] uint32_t GetStartSlot() const noexcept override { return startSlot; }

    private:
        friend class DescriptorHeap;

        uint32_t startSlot;
        uint32_t slotCount;
        uint32_t descriptorLength;
        bool shaderVisible;
        std::set<uint32_t> freeSlots;

        DescriptorHeap* parentHeap;
    };
}

#endif //VERTIX_DESCRIPTORRANGE_H
