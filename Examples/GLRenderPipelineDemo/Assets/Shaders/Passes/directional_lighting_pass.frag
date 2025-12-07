#version 450 core

layout (location = 0) out vec4 gShadow;

in vec2 TexCoord;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;

layout (binding = 2) uniform sampler2D shadowMap;

uniform vec3 lightDirection;
uniform mat4 lightSpaceMatrix;

void main() {
    vec3 fragPos = texture(gPosition, TexCoord).rgb;
    vec3 normal = texture(gNormal, TexCoord).rgb;

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;

    gShadow = vec4(vec3(1 - shadow), 1.0);
}