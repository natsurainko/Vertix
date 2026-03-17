//
// Created by Natsurainko on 2026/3/15.
//

#ifndef VERTIX_RESOURCEHANDLE_H
#define VERTIX_RESOURCEHANDLE_H

#include <cstdint>

namespace Vertix {
    template<typename Tag>
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