#ifndef CONSTANTS_BUFFER_STRUCTS_H
#define CONSTANTS_BUFFER_STRUCTS_H

#include "chlsl.h"

#ifndef __cplusplus
struct MaterialConstants {
    uint albedoHandle;
    uint normalHandle;
    uint metallicHandle;
    uint roughnessHandle;

    uint  ambientOcclusionHandle;
    uint  emissiveHandle;
    float metallicFactor;
    float roughnessFactor;

    uint flags;
    uint padding[3];
};
#endif

struct FrameConstants {
    float4x4 View              IDENTITY;
    float4x4 Projection        IDENTITY;
    float4x4 ViewProjection    IDENTITY;
    float4x4 InvViewProjection IDENTITY;

    float4 CameraPosition;
};

struct LightConstants {
    float3 LightDirection;
    float  AmbientIntensity;

    float3 LightColor;
    float  LightIntensity;
};

struct ObjectConstants {
    float4x4 World                 IDENTITY;
    float4x4 WorldInverseTranspose IDENTITY;
};

#endif // CONSTANTS_BUFFER_STRUCTS_H