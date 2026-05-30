//
// Created by Natsurainko on 2026/3/10.
//

#pragma once

#include "Vertix/Pool/ResourceHandle.h"

namespace Vertix {
    struct MaterialTag {};

    using MaterialHandle = ResourceHandle<MaterialTag>;

    struct Material {
        virtual ~Material() = default;
    };
}
