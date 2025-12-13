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
    const float biasModifier = 0.5f;

    float shadow = 0.0;
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
        
    return shadow;
}

void main() {
    float shadow = CalculateShadow();
    gShadow = vec4(vec3(shadow), 1.0);
}