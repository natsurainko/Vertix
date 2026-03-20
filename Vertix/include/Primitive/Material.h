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
        uint32_t albedoHandle        = 0;
        uint32_t normalHandle        = 0;
        uint32_t metallicHandle      = 0;
        uint32_t roughnessHandle     = 0;

        uint32_t ambientOcclusionHandle = 0;
        uint32_t emissiveHandle         = 0;
        float    metallicFactor         = 1.f;
        float    roughnessFactor        = 1.f;

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
            out.albedoHandle = albedo.slot;
            out.normalHandle = normal.slot;
            out.metallicHandle = metallic.slot;
            out.roughnessHandle = roughness.slot;
            out.ambientOcclusionHandle = ambientOcclusion.slot;
            out.emissiveHandle = emissive.slot;
            out.metallicFactor = metallicFactor;
            out.roughnessFactor = roughnessFactor;
        }
    };
}

#endif //VERTIX_MATERIAL_H