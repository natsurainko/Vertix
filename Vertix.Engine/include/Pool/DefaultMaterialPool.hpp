//
// Created by Natsurainko on 2026/4/10.
//

#ifndef VERTIX_DEFAULTMATERIALPOOL_H
#define VERTIX_DEFAULTMATERIALPOOL_H

#include "Pool/MaterialPool.hpp"
#include "Primitive/DefaultPBRMaterial.h"

namespace Vertix::Engine {
    template<uint32_t Capacity = 1024>
    class DefaultMaterialPool : public MaterialPool<DefaultMaterialConstants, Capacity> {
    public:
        explicit DefaultMaterialPool(const GraphicsDevice* graphicsDevice) : MaterialPool<DefaultMaterialConstants, Capacity>(graphicsDevice) {}
    };
}

#endif //VERTIX_DEFAULTMATERIALPOOL_H