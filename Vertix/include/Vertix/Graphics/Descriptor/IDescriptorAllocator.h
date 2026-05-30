//
// Created by Natsurainko on 2026/5/17.
//

#pragma once

#include "DescriptorHandle.h"

namespace Vertix {
    struct IDescriptorAllocator {
        virtual ~IDescriptorAllocator() = default;

        virtual DescriptorHandle AllocDescriptorHandle() = 0;
        virtual void             FreeDescriptorHandle(const DescriptorHandle &handle) = 0;

        virtual bool     IsFull() const noexcept = 0;
        virtual uint32_t GetFreeCount() const noexcept = 0;
        virtual uint32_t GetTotalCount() const noexcept = 0;
        virtual uint32_t GetStartSlot() const noexcept = 0;
    };
}
