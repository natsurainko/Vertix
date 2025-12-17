#version 450 core

layout (location = 0) out float gShadow;

in vec2 TexCoord;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;

layout (binding = 2) uniform sampler2DArray shadowMap;
layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[8];
};

uniform vec3 lightDirection;
uniform float cascadePlaneDistances[8];
uniform int cascadeCount;
uniform vec2 shadowMapTexelSize;

uniform float lightSize = 0.15;

const vec2 poissonDisk[32] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845),
    vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790),
    vec2(-0.61342514, 0.61513743),
    vec2(0.17088780, -0.04025297),
    vec2(0.64568018, 0.49258912),
    vec2(-0.70537376, -0.66820324),
    vec2(0.06332609, 0.14236987),
    vec2(0.20352896, 0.21433118),
    vec2(-0.88592213, 0.21536890),
    vec2(0.56663656, 0.60521287),
    vec2(0.03976609, -0.39610012),
    vec2(-0.46757436, -0.40543765),
    vec2(-0.24826765, -0.81475324),
    vec2(0.35441089, -0.88757098),
    vec2(0.48787654, -0.06308210),
    vec2(-0.69689012, -0.54979102),
    vec2(0.03421098, 0.97998012),
    vec2(0.50309765, -0.30887654)
);

vec2 RotateVector(vec2 v, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return vec2(v.x * c - v.y * s, v.x * s + v.y * c);
}

float CalculateReceiverPlaneBias(vec3 fragPos, vec3 normal, int layer) {
    vec3 lightDir = normalize(-lightDirection);

    vec4 shadowPos = lightSpaceMatrices[layer] * vec4(fragPos, 1.0);
    vec3 shadowCoord = shadowPos.xyz / shadowPos.w;
    
    vec3 dx = dFdx(shadowCoord);
    vec3 dy = dFdy(shadowCoord);
    
    float dudx = dx.x * shadowMapTexelSize.x;
    float dudy = dy.x * shadowMapTexelSize.y;
    float dvdx = dx.y * shadowMapTexelSize.x;
    float dvdy = dy.y * shadowMapTexelSize.y;
    
    float biasScale = max(abs(dudx), abs(dudy));
    float normalBias = clamp(1.0 - dot(normal, lightDir), 0.0, 1.0);
    
    return biasScale * normalBias * 2.0 + 0.0002;
}

float FindBlockDepth(vec3 projCoords, float layer) {
    const float c = 75.0;

    float searchWidth = lightSize * projCoords.z * c;
    float blockerDepth = 0.01; // * projCoords.z;
    float blockerCount = 0.01;

    for (int i = 0; i < 16; ++i) {
        vec2 offset = poissonDisk[i] * shadowMapTexelSize * searchWidth;
        float depth = texture(shadowMap, vec3(projCoords.xy + offset, layer)).r;
        
        if (depth < projCoords.z) {
            blockerDepth += depth;
            blockerCount += 1.0;
        }
    }

    if (blockerCount <= 1) return -1.0;
    return blockerDepth / blockerCount;
}

float PossionSampleShadow(vec3 projCoords, float layer, float bias, float searchRadius) {
    float angle = fract(sin(dot(TexCoord, vec2(12.9898, 78.233))) * 43758.5453) * 6.2831; //8530718;
    float shadow = 0.0;

    for(int i = 16; i < 32; ++i) {
        vec2 rotatedOffset = RotateVector(poissonDisk[i], angle);
        vec2 offset = rotatedOffset * searchRadius;
        float pcfDepth = texture(shadowMap, vec3(projCoords.xy + offset, layer)).r;
        shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;
    }
    
    return shadow / 16.0;
}

float CalculateShadow(vec3 fragPos, vec3 normal, float depth) {
    int layer = cascadeCount;
    for (int i = 0; i < cascadeCount; ++i) {
        if (depth < cascadePlaneDistances[i]) {
            layer = i;
            break;
        }
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    float bias = CalculateReceiverPlaneBias(fragPos, normal, layer);
    float blockDepth = FindBlockDepth(projCoords, layer);

    if (blockDepth < 0.0) return 0.0;

    float penumbraWidth = (projCoords.z - blockDepth) / blockDepth * lightSize;
    penumbraWidth = max(0.0005, penumbraWidth);

    return PossionSampleShadow(projCoords, layer, bias, penumbraWidth);
}

void main() {
    vec4 fragPos = texture(gPosition, TexCoord);
    if (fragPos.a < 0) return;
    vec3 normal = texture(gNormal, TexCoord).rgb;

    gShadow = CalculateShadow(fragPos.rgb, normal, fragPos.a);
}