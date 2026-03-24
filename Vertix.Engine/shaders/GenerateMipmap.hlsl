SamplerState        MipMapSampler : register(s0);
Texture2D<float4>   InputMipmap   : register(t0);
RWTexture2D<float4> OutputMipmap  : register(u0);

cbuffer MipConstants : register(b0)
{
    float2 OutputMipmapTexelSize;
    uint   InputMipmapLevel;
}

[numthreads(8, 8, 1)]
void CSMain(uint3 threadId : SV_DispatchThreadID) {
    float2 uv = (threadId.xy + 0.5) * OutputMipmapTexelSize;
    OutputMipmap[threadId.xy] = InputMipmap.SampleLevel(MipMapSampler, uv, InputMipmapLevel);
}