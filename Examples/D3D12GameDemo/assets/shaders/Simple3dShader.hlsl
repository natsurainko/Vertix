cbuffer root : register(b0) {
    float4x4 WorldViewProjectionMatrix;
    float4x4 WorldInverseTranspose;
}

static const float3 lightDir = normalize(float3(1.0, 1.0, 1.0));
static const float3 lightColor = float3(1.0, 1.0, 1.0);

struct VSInput {
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    // float2 TexCoord : TEXCOORD;
};

struct VSOutput {
    // float4 FragPos : SV_POSITION;
    float4 Position : SV_POSITION;
    float3 FragNormal : NORMAL;
    // float2 TexCoord : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput) {
    VSOutput output;
    
    output.Position = mul(WorldViewProjectionMatrix, float4(vsInput.Position, 1.0f));
    output.FragNormal = mul((float3x3)WorldInverseTranspose, vsInput.Normal);

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target {
    float3 normal = normalize(psInput.FragNormal);
    float diff = max(dot(normal, lightDir), 0.0);

    float3 diffuse = diff * lightColor;
    float3 ambient = 0.3 * lightColor;

    return float4(diffuse + ambient, 1.0);
}