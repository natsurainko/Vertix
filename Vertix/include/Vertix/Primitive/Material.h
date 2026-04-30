//
// Created by Natsurainko on 2026/3/10.
//

#ifndef VERTIX_MATERIAL_H
#define VERTIX_MATERIAL_H

#include "Vertix/Pool/ResourceHandle.h"

namespace Vertix {
    struct MaterialTag{};
    using MaterialHandle = ResourceHandle<MaterialTag>;

    struct Material {
        virtual ~Material() = default;
    };
}

#endif //VERTIX_MATERIAL_H
