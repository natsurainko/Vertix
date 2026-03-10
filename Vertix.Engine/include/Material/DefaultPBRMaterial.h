//
// Created by Natsurainko on 2026/2/18.
//

#ifndef VERTIX_DEFAULTPBRMATERIAL_H
#define VERTIX_DEFAULTPBRMATERIAL_H

#include "Primitive/Material.h"
#include "Rendering/Texture.h"

namespace Vertix::Engine {
    struct DefaultPBRMaterial : Material {
        Texture* AlbedoTexture = nullptr;
        Texture* MetallicTexture = nullptr;
        Texture* RoughnessTexture = nullptr;
        Texture* NormalTexture = nullptr;
        Texture* AoTexture = nullptr;
    };
}

#endif //VERTIX_DEFAULTPBRMATERIAL_H