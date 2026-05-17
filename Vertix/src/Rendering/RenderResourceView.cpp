//
// Created by Natsurainko on 2026/5/16.
//

#include "Vertix/Rendering/RenderResourceView.h"

bool Vertix::RenderResourceViewDesc::operator==(const RenderResourceViewDesc &other) const {
    if (type != other.type) return false;
    if (desc.index() != other.desc.index()) return false;

    return std::visit([&]<typename T>(const T& lhs) -> bool {
        const T& rhs = std::get<T>(other.desc);
        if constexpr (std::is_same_v<T, std::monostate>) {
            return true;
        } else {
            return std::memcmp(&lhs, &rhs, sizeof(T)) == 0;
        }
    }, desc);
}

size_t std::hash<Vertix::RenderResourceViewDesc>::operator()(const Vertix::RenderResourceViewDesc &desc) const noexcept {
    size_t seed = 0;

    auto hashCombine = [&seed](auto&&... vals) {
        // Boost-style hash_combine
        auto combine = [&seed](size_t h) {
            seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };
        (combine(std::hash<std::decay_t<decltype(vals)>>{}(vals)), ...);
    };

    // Hash type enum
    hashCombine(static_cast<int>(desc.type));

    // Hash variant: combine index + raw bytes of the active descriptor
    hashCombine(desc.desc.index());
    std::visit([&](const auto& d) {
        using T = std::decay_t<decltype(d)>;
        if constexpr (std::is_same_v<T, D3D12_SHADER_RESOURCE_VIEW_DESC>) {
            hashCombine(d.Format, d.ViewDimension, d.Shader4ComponentMapping);
        } else if constexpr (std::is_same_v<T, D3D12_UNORDERED_ACCESS_VIEW_DESC>) {
            hashCombine(d.Format, d.ViewDimension);
        } else if constexpr (std::is_same_v<T, D3D12_RENDER_TARGET_VIEW_DESC>) {
            hashCombine(d.Format, d.ViewDimension);
        }
    }, desc.desc);

    return seed;
}
