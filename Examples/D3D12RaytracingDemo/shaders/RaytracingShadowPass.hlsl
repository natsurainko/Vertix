#include "structures.h"

ConstantBuffer<LightConstants> lightConstants : register(b0);

RaytracingAccelerationStructure gRtScene        : register(t0);
RWTexture2D<float>              gShadowMask     : register(u0);

SamplerState NearestSampler : register(s0);

Texture2D<float4> gPositionDepth   : register(t1);
Texture2D<float4> gNormalRoughness : register(t2);

struct ShadowRayPayload {
    float visible;
    float t;
};

[shader("miss")]
void ShadowMiss(inout ShadowRayPayload payload) {
    payload.visible = 1.0f;
    payload.t       = 0.0;
}

[shader("closesthit")]
void ShadowClosestHit(inout ShadowRayPayload payload, in BuiltInTriangleIntersectionAttributes attribs) {
    payload.visible = 0.f;
    payload.t       = RayTCurrent();
}

static const int   BLOCKER_SAMPLES  = 4;
static const int   SHADOW_SAMPLES   = 8;
static const float LIGHT_RADIUS     = 1.0f;

uint WangHash(uint seed) {
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u; seed ^= seed >> 4u;
    seed *= 0x27d4eb2du; seed ^= seed >> 15u;
    return seed;
}
float RandFloat(inout uint state) {
    state = WangHash(state);
    return float(state) * (1.0 / 4294967296.0);
}
uint InitRNG(uint2 pixel) {
    return WangHash(pixel.x + pixel.y * 8192u);
}
float2 SampleDisk(float2 xi) {
    float2 o = 2.0f * xi - 1.0f;
    if (o.x == 0 && o.y == 0) return 0;
    float r, theta;
    if (abs(o.x) > abs(o.y)) { r = o.x; theta = (3.14159265f/4) * (o.y/o.x); }
    else                     { r = o.y; theta = (3.14159265f/2) - (3.14159265f/4) * (o.x/o.y); }
    return r * float2(cos(theta), sin(theta));
}
void BuildTangentFrame(float3 L, out float3 T, out float3 B) {
    float3 up = abs(L.y) < 0.999f ? float3(0,1,0) : float3(1,0,0);
    T = normalize(cross(up, L));
    B = cross(L, T);
}

[shader("raygeneration")]
void RayGen() {
    ShadowRayPayload payload = { 0.f, 0.f };

    uint2  index = DispatchRaysIndex().xy;
    float2 uv    = float2(index + .5f) / DispatchRaysDimensions().xy;

    float4 positionDepth = gPositionDepth.SampleLevel(NearestSampler, uv, 0);
    if (positionDepth.w <= 0) {
        gShadowMask[index.xy] = payload.visible;
        return;
    }

    float  depth   = positionDepth.w;
    float3 postion = positionDepth.xyz;
    float3 normal  = gNormalRoughness.SampleLevel(NearestSampler, uv, 0).rgb;

    float3 L     = -lightConstants.LightDirection;
    float  NdotL = max(dot(normal, L), 0.0f);

    if (NdotL <= 0.0f) {
        gShadowMask[index.xy] = 0.0f;
        return;
    }

    float3 shadowOrigin = postion + normal * 1e-3f;
    float lightDistance = 50.0f;

    float3 T, B;
    BuildTangentFrame(L, T, B);

    uint rng = InitRNG(index);

    // Phase 1: Blocker Search
    float blockerSum   = 0.0f;
    int   blockerCount = 0;

    for (int i = 0; i < BLOCKER_SAMPLES; ++i) {
        float2 xi   = float2(RandFloat(rng), RandFloat(rng));
        float2 disk = SampleDisk(xi) * LIGHT_RADIUS * 2.0f;

        float3 lightCenter = shadowOrigin + L * lightDistance;
        float3 lightSample = lightCenter + disk.x * T + disk.y * B;

        ShadowRayPayload blockerPayload = { 0.f, 0.f };

        RayDesc blockerRayDesc;
        blockerRayDesc.Origin    = shadowOrigin;
        blockerRayDesc.Direction = normalize(lightSample - shadowOrigin);
        blockerRayDesc.TMin      = 0.0f;
        blockerRayDesc.TMax      = 1e4f;

        TraceRay(gRtScene,
            RAY_FLAG_CULL_BACK_FACING_TRIANGLES      |
            RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH |
            RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES      |
            RAY_FLAG_CULL_NON_OPAQUE,
            0xFF,
            0,
            0,
            0,
            blockerRayDesc,
            blockerPayload
        );

        if (blockerPayload.visible == 0.0f) {
            blockerSum += blockerPayload.t;
            blockerCount++;
        }
    }

    if (blockerCount == BLOCKER_SAMPLES) {
        gShadowMask[index.xy] = 0.0f;
        return;
    }
    if (blockerCount == 0) {
        gShadowMask[index.xy] = 1.0f;
        return;
    }

    // Phase 2: Penumbra Size
    float avgBlockerDepth = blockerSum / float(blockerCount);
    float receiverDepth   = lightDistance;

    float penumbra = (receiverDepth - avgBlockerDepth) / avgBlockerDepth;
    penumbra       = max(0.0f, penumbra);
    penumbra       *= LIGHT_RADIUS;
    penumbra       = min(penumbra, LIGHT_RADIUS * 2.0f);

    float litCount = 0.0f;
    for (int i = 0; i < SHADOW_SAMPLES; ++i) {
        float2 xi   = float2(RandFloat(rng), RandFloat(rng));
        float2 disk = SampleDisk(xi) * penumbra;

        float3 lightCenter = shadowOrigin + L * lightDistance;
        float3 lightSample = lightCenter + disk.x * T + disk.y * B;

        ShadowRayPayload samplePayload = { 0.f, 0.f };

        RayDesc sampleRayDesc;
        sampleRayDesc.Origin    = shadowOrigin;
        sampleRayDesc.Direction = normalize(lightSample - shadowOrigin);
        sampleRayDesc.TMin      = 0.0f;
        sampleRayDesc.TMax      = 1e4f;

        TraceRay(gRtScene,
            RAY_FLAG_CULL_BACK_FACING_TRIANGLES      |
            RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH |
            RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES      |
            RAY_FLAG_CULL_NON_OPAQUE,
            0xFF,
            0,
            0,
            0,
            sampleRayDesc,
            samplePayload
        );

        litCount += (samplePayload.visible > 0.0f) ? 1.0f : 0.0f;
    }

    gShadowMask[index.xy] = litCount / float(SHADOW_SAMPLES);
}