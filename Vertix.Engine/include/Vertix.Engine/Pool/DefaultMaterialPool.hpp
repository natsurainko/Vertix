//
// Created by Natsurainko on 2026/4/10.
//

#ifndef VERTIX_DEFAULTMATERIALPOOL_H
#define VERTIX_DEFAULTMATERIALPOOL_H

#include <Vertix/Pool/MaterialPool.hpp>

#include "Vertix.Engine/Primitive/DefaultPBRMaterial.h"

namespace Vertix::Engine {
    class DefaultMaterialPool : public MaterialPool<DefaultMaterialConstants> {
    public:
        explicit DefaultMaterialPool(
            const GraphicsDevice* graphicsDevice,
            const uint32_t capacity)
        : MaterialPool(graphicsDevice, capacity) {}
    };
}

#endif //VERTIX_DEFAULTMATERIALPOOL_H
