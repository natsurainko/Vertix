#version 450 core

layout (location = 0) out vec4 gShadow;

in vec2 TexCoord;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;

layout (binding = 2) uniform sampler2DArray shadowMap;
layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[8];
};

uniform mat4 view;
uniform vec3 lightDirection;
uniform float cascadePlaneDistances[8];
uniform int cascadeCount;
uniform float farPlane;

const vec2 poissonDisk[16] = vec2[](
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
    vec2(0.14383161, -0.14100790)
);

float CalculateReceiverPlaneBias(vec3 fragPos, vec3 normal, int layer, vec2 texelSize) {
    vec3 lightDir = normalize(-lightDirection);

    vec4 shadowPos = lightSpaceMatrices[layer] * vec4(fragPos, 1.0);
    vec3 shadowCoord = shadowPos.xyz / shadowPos.w;
    
    vec3 dx = dFdx(shadowCoord);
    vec3 dy = dFdy(shadowCoord);
    
    float dudx = dx.x * texelSize.x;
    float dudy = dy.x * texelSize.y;
    float dvdx = dx.y * texelSize.x;
    float dvdy = dy.y * texelSize.y;
    
    float biasScale = max(abs(dudx), abs(dudy));
    float normalBias = clamp(1.0 - dot(normal, lightDir), 0.0, 1.0);
    
    return biasScale * normalBias * 2.0 + 0.0002;
}

float PossionSampleShadow(vec3 projCoords, float layer, float depth, float bias, vec2 texelSize) {
    float searchRadius = 2.0;
    float shadow = 0.0;

    for(int i = 0; i < 16; ++i) {
        vec2 offset = poissonDisk[i] * texelSize * searchRadius;
        float pcfDepth = texture(shadowMap, vec3(projCoords.xy + offset, layer)).r;
        shadow += (depth - bias) > pcfDepth ? 1.0 : 0.0;
    }
    
    return shadow / 16.0;
}

float CalculateShadow() {
    vec3 fragPos = texture(gPosition, TexCoord).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoord).rgb);
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));

    vec4 fragPosViewSpace = view * vec4(fragPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = cascadeCount;
    for (int i = 0; i < cascadeCount; ++i) {
        if (depthValue < cascadePlaneDistances[i]) {
            layer = i;
            break;
        }
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    if (currentDepth > 1.0) return 0.0;

    float bias = CalculateReceiverPlaneBias(fragPos, normal, layer, texelSize);
    return PossionSampleShadow(projCoords, layer, currentDepth, bias, texelSize);
}

void main() {
    float shadow = CalculateShadow();
    gShadow = vec4(vec3(shadow), 1.0);
}