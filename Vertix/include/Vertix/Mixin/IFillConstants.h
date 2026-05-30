//
// Created by Natsurainko on 2026/3/15.
//

#pragma once

namespace Vertix {
    template <typename TConstants>
    struct IFillConstants {
        virtual void Fill(TConstants &out) const = 0;
        virtual      ~IFillConstants() = default;
    };
}
