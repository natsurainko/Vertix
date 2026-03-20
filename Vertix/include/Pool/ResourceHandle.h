//
// Created by Natsurainko on 2026/3/15.
//

#ifndef VERTIX_RESOURCEHANDLE_H
#define VERTIX_RESOURCEHANDLE_H

#include <cstdint>

namespace Vertix {
    // All ResourceHandles follow a 1-based handle rule:
    // A handle with a slot of 0 represent an invalid resource.
    // However, during actual resource filling, to utilize space as much as possible,
    // slot - 1 is used as the index to fill shader resources such as DescriptorHeap or StructuredBuffer.
    // Therefore, you should also follow this convention when indexing in your shaders.
    template<typename>
    struct ResourceHandle {
        static constexpr std::uint32_t Null = 0;
        uint32_t slot = Null;

        explicit operator bool() const noexcept { return slot; }
        bool operator==(const ResourceHandle & handle) const noexcept { return slot == handle.slot; }
    };
}

template<typename Tag>
struct std::hash<Vertix::ResourceHandle<Tag>> {
    std::size_t operator()(const Vertix::ResourceHandle<Tag>& handle) const noexcept {
        return handle.slot;
    }
};

#endif //VERTIX_RESOURCEHANDLE_H