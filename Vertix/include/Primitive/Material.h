//
// Created by Natsurainko on 2026/3/10.
//

#ifndef VERTIX_MATERIAL_H
#define VERTIX_MATERIAL_H

#include "Texture.h"
#include "Mixin/IFillConstants.h"
#include "Pool/ResourceHandle.h"

namespace Vertix {
    struct MaterialTag{};
    using MaterialHandle = ResourceHandle<MaterialTag>;

    struct alignas(16) DefaultMaterialConstants {
        uint32_t albedoIndex        = 0;
        uint32_t normalIndex        = 0;
        uint32_t metallicIndex      = 0;
        uint32_t roughnessIndex     = 0;

        uint32_t ambientOcclusionIndex = 0;
        uint32_t emissiveIndex         = 0;
        float    metallicFactor        = 1.f;
        float    roughnessFactor       = 1.f;

        uint32_t flags = 0;
        uint32_t padding[3];
    };

    struct Material {
        virtual ~Material() = default;
    };

    struct DefaultPBRMaterial : Material, IFillConstants<DefaultMaterialConstants> {
        TextureHandle albedo;
        TextureHandle normal;
        TextureHandle metallic;
        TextureHandle roughness;
        TextureHandle ambientOcclusion;
        TextureHandle emissive;

        float metallicFactor  = 1.f;
        float roughnessFactor = 1.f;

        void Fill(DefaultMaterialConstants& out) const override {
            out.albedoIndex = albedo.slot;
            out.normalIndex = normal.slot;
            out.metallicIndex = metallic.slot;
            out.roughnessIndex = roughness.slot;
            out.ambientOcclusionIndex = ambientOcclusion.slot;
            out.emissiveIndex = emissive.slot;
            out.metallicFactor = metallicFactor;
            out.roughnessFactor = roughnessFactor;
        }
    };
}

#endif //VERTIX_MATERIAL_H