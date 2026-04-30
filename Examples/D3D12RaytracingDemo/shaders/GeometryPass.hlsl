#include "structures.h"

// ====================================================
//                    Vertex Shader
// ====================================================

ConstantBuffer<FrameConstants>  frameConstants  : register(b0);
ConstantBuffer<ObjectConstants> objectConstants : register(b1);

struct VSInput {
    float3 Position  : POSITION;
    float3 Normal    : NORMAL;
};

struct VSOutput {
    float4 FragPos    : POSITION;
    float3 FragNormal : NORMAL;
    float4 Position   : SV_POSITION;
};

VSOutput VSMain(VSInput vsInput) {
    float4 worldPos = mul(objectConstants.World, float4(vsInput.Position, 1.0f));
    float4 viewPos  = mul(frameConstants.View, worldPos);

    VSOutput output;
    output.FragPos    = float4(worldPos.xyz, -viewPos.z);
    output.FragNormal = mul((float3x3)objectConstants.WorldInverseTranspose, vsInput.Normal);
    output.Position   = mul(frameConstants.Projection, viewPos);
    return output;
}

// ====================================================
//                    Pixel Shader
// ====================================================

struct PSOutput {
    float4 GPositionDepth   : SV_Target0;
    float4 GNormalRoughness : SV_Target1;
};

PSOutput PSMain(VSOutput psInput) {
    PSOutput output;
    output.GPositionDepth   = psInput.FragPos;
    output.GNormalRoughness = float4(normalize(psInput.FragNormal), 0.0f);
    return output;
}
