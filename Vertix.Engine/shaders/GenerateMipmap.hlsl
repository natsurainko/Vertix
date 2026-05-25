SamplerState        MipMapSampler : register(s0);
Texture2D<float4>   InputMipmap   : register(t0);
RWTexture2D<float4> OutputMipmap  : register(u0);

cbuffer MipConstants : register(b0)
{
    float2 OutputMipmapTexelSize;
    uint   InputMipmapLevel;
    uint   IsSRGBTexture;
}

[numthreads(8, 8, 1)]
void CSMain(uint3 threadId : SV_DispatchThreadID) {
    float2 uv = (threadId.xy + 0.5) * OutputMipmapTexelSize;
    float4 color = InputMipmap.SampleLevel(MipMapSampler, uv, InputMipmapLevel);
    if (IsSRGBTexture) color = pow(color, 1.0 / 2.2);
    OutputMipmap[threadId.xy] = color;
}
