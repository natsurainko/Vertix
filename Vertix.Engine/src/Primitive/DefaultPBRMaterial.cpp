//
// Created by Natsurainko on 2026/4/10.
//

#include "Vertix.Engine/Primitive/DefaultPBRMaterial.h"

#include <map>
#include <assimp/GltfMaterial.h>
#include <assimp/material.h>
#include <DirectXTK12/WICTextureLoader.h>

void Vertix::Engine::DefaultPBRMaterial::ReadPropertiesFromAssimp(const aiMaterial* material) {
    material->Get(AI_MATKEY_BASE_COLOR, *reinterpret_cast<aiColor4D*>(this->baseColorFactor));
    material->Get(AI_MATKEY_METALLIC_FACTOR, this->metallicFactor);
    material->Get(AI_MATKEY_ROUGHNESS_FACTOR, this->roughnessFactor);

    aiColor4D aiEmissiveFactor;
    float     aiEmissiveStrength;
    material->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmissiveFactor);
    material->Get(AI_MATKEY_EMISSIVE_INTENSITY, aiEmissiveStrength);

    this->emissiveFactor[0] = aiEmissiveFactor.r;
    this->emissiveFactor[1] = aiEmissiveFactor.b;
    this->emissiveFactor[2] = aiEmissiveFactor.g;
    // TODO: Implement emissiveStrength in KHR_materials_emissive_strength extension

    material->Get(AI_MATKEY_GLTF_TEXTURE_STRENGTH(aiTextureType_LIGHTMAP, 0), this->occlusionStrength);
    material->Get(AI_MATKEY_GLTF_TEXTURE_SCALE(aiTextureType_NORMALS, 0), this->normalScale);

    aiString aiAlphaMode;
    material->Get(AI_MATKEY_GLTF_ALPHAMODE, aiAlphaMode);
    material->Get(AI_MATKEY_GLTF_ALPHACUTOFF, this->alphaCutoff);
    material->Get(AI_MATKEY_TWOSIDED, this->doubleSided);

    if (const auto aiAlphaModeString = std::string(aiAlphaMode.C_Str()); aiAlphaModeString == "MASK") {
        this->alphaMode = 1;
    } else if (aiAlphaModeString == "BLEND") {
        this->alphaMode = 2;
    } else {
        this->alphaMode = 0;
    }
}

void Vertix::Engine::DefaultPBRMaterial::ReadTexturesFromAssimp(
    const aiMaterial* material,
    const std::function<void(aiString, aiTextureType, TextureHandle*)> &textureCallback)
{
    const std::map<aiTextureType, TextureHandle*> textureTypeToPtr = {
        { aiTextureType_BASE_COLOR, &this->baseColorTexture },
        { aiTextureType_DIFFUSE, &this->baseColorTexture },
        { aiTextureType_GLTF_METALLIC_ROUGHNESS, &this->metallicRoughnessTexture },
        { aiTextureType_NORMALS, &this->normalTexture },
        { aiTextureType_LIGHTMAP, &this->occlusionTexture },
        { aiTextureType_EMISSIVE, &this->emissiveTexture }
    };

    for (const auto &[aiTexType, destPtr] : textureTypeToPtr) {
        aiString path;
        if (const aiReturn result = material->GetTexture(aiTexType, 0, &path); result != AI_SUCCESS) continue;
        textureCallback(path, aiTexType, destPtr);
    }
}

DirectX::WIC_LOADER_FLAGS Vertix::Engine::DefaultPBRMaterial::GetWicLoaderFlags(const aiTextureType aiTextureType) {
    switch (aiTextureType) {
        case aiTextureType_BASE_COLOR:
        case aiTextureType_DIFFUSE:
        case aiTextureType_LIGHTMAP:
            return DirectX::WIC_LOADER_FORCE_SRGB | DirectX::WIC_LOADER_FORCE_RGBA32;
        default:
            return DirectX::WIC_LOADER_IGNORE_SRGB | DirectX::WIC_LOADER_FORCE_RGBA32;
    }
}
