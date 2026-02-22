//
// Created by Natsurainko on 2026/2/18.
//

#ifndef VERTIX_DEFAULTPBRMATERIAL_H
#define VERTIX_DEFAULTPBRMATERIAL_H

#include "Material.h"
#include "Rendering/Texture.h"

namespace Vertix::Engine {
    struct DefaultPBRMaterial : Material {
        Texture* DiffuseTexture = nullptr;
        Texture* MetallicTexture = nullptr;
        Texture* RoughnessTexture = nullptr;
        Texture* NormalTexture = nullptr;
    };
}

#endif //VERTIX_DEFAULTPBRMATERIAL_H