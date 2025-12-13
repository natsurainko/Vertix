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

float CalculateShadow() {
    vec3 fragPos = texture(gPosition, TexCoord).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoord).rgb);

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

    //float bias = max(0.05 * (1.0 - dot(normal, -lightDirection)), 0.005);
    float bias = 0.00125;
    const float biasModifier = 0.15f;

    if (layer == cascadeCount)
        bias *= 1 / (farPlane * biasModifier);
    else
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
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