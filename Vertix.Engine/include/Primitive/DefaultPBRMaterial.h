//
// Created by Natsurainko on 2026/4/10.
//

#ifndef VERTIX_DEFAULTPBRMATERIAL_H
#define VERTIX_DEFAULTPBRMATERIAL_H

#include <functional>
#include <assimp/material.h>
#include <DirectXTK12/WICTextureLoader.h>

#include "VERTIX_ENGINE_EXPORT.h"
#include "Mixin/IFillConstants.h"
#include "Primitive/Material.h"
#include "Primitive/Texture.h"

namespace Vertix::Engine {
    /// DefaultMaterialConstants is a shader constant structure designed for DefaultPBRMaterial.
    struct alignas(16) DefaultMaterialConstants {
        uint32_t baseColorHandle         = 0;
        uint32_t metallicRoughnessHandle = 0;
        uint32_t normalHandle            = 0;
        uint32_t occlusionHandle         = 0;

        uint32_t emissiveHandle  = 0;
        float    metallicFactor  = 1.f;
        float    roughnessFactor = 1.f;
        float    normalScale     = 1.f;

        float occlusionStrength = 1.f;
        float emissiveFactor[3] = { 1.f, 1.f, 1.f };

        float baseColorFactor[4] = { 1.f, 1.f, 1.f, 1.f };

        uint32_t alphaMode   = 0;
        float    alphaCutoff = 0.5f;
        int      doubleSided = false;
        float    padding     = {};
    };

    /// DefaultPBRMaterial are material implemented based on the glTF specification.
    /// Please refer to the glTF implementation specification when compositing BRDFs.
    struct VERTIX_ENGINE_API DefaultPBRMaterial : Material, IFillConstants<DefaultMaterialConstants> {
        /// float4 RGBA sRGB Texture
        TextureHandle baseColorTexture;

        /// float3 RGB Linear Texture: G for roughness, B for metallic
        TextureHandle metallicRoughnessTexture;

        /// float3 XYZ Linear Texture: normal = px * 2.0 - 1.0
        TextureHandle normalTexture;

        /// float R Linear Texture
        TextureHandle occlusionTexture;

        /// float3 RGB sRGB Texture
        TextureHandle emissiveTexture;

        float metallicFactor     = 1.f;
        float roughnessFactor    = 1.f;
        float normalScale        = 1.f;
        float occlusionStrength  = 1.f;
        float emissiveFactor[3]  = { 1.f, 1.f, 1.f };
        float baseColorFactor[4] = { 1.f, 1.f, 1.f, 1.f };

        /// 0 -> OPAQUE ; 1 -> MASK ; 2 -> BLEND
        uint32_t alphaMode   = 0;

        /// When alphaMode is MASK, clipping will be performed based on alphaCutoff.
        float alphaCutoff = 0.5f;

        /// This is a double-sided material and should disable culling.
        bool doubleSided = false;

        void Fill(DefaultMaterialConstants& out) const override {
            out.baseColorHandle         = baseColorTexture.slot;
            out.metallicRoughnessHandle = metallicRoughnessTexture.slot;
            out.normalHandle            = normalTexture.slot;
            out.occlusionHandle         = occlusionTexture.slot;

            out.emissiveHandle  = emissiveTexture.slot;
            out.metallicFactor  = metallicFactor;
            out.roughnessFactor = roughnessFactor;
            out.normalScale     = normalScale;

            out.occlusionStrength = occlusionStrength;
            out.emissiveFactor[0] = emissiveFactor[0];
            out.emissiveFactor[1] = emissiveFactor[1];
            out.emissiveFactor[2] = emissiveFactor[2];

            out.baseColorFactor[0] = baseColorFactor[0];
            out.baseColorFactor[1] = baseColorFactor[1];
            out.baseColorFactor[2] = baseColorFactor[2];
            out.baseColorFactor[3] = baseColorFactor[3];

            out.alphaMode   = alphaMode;
            out.alphaCutoff = alphaCutoff;
            out.doubleSided = doubleSided;
        }

        void ReadPropertiesFromAssimp(const aiMaterial* material);

        void ReadTexturesFromAssimp(
            const aiMaterial* material,
            const std::function<void(aiString, aiTextureType, TextureHandle*)> &textureCallback);

        static DirectX::WIC_LOADER_FLAGS GetWicLoaderFlags(aiTextureType aiTextureType);
    };
}

#endif //VERTIX_DEFAULTPBRMATERIAL_H