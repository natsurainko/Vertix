// ====================================================
//                    Vertex Shader
// ====================================================

struct VSInput {
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct VSOutput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput) {
    VSOutput output;
    output.Position = float4(vsInput.Position, 1.0);
    output.TexCoord = vsInput.TexCoord;
    return output;
}

// ====================================================
//                    Pixel Shader
// ====================================================

SamplerState LinearSampler : register(s0);

Texture2D<float4> gShadowMask : register(t0);

float4 PSMain(VSOutput psInput) : SV_TARGET0 {
    float  shadow = gShadowMask.Sample(LinearSampler, psInput.TexCoord).r;
    return float4(shadow, shadow, shadow, 1.0);
}