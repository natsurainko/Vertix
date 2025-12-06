#version 450 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

layout (binding = 0) uniform sampler2D texture_diffuse;
layout (binding = 1) uniform sampler2D texture_specular;

uniform vec4 baseColor = vec4(1.0);
uniform float roughness = 0.5;
uniform float metallic = 0.0;

void main() {
	gPosition = vec4(FragPos, 1.0);
	gNormal = vec4(normalize(Normal), roughness);
	gAlbedoSpec.rgb = texture(texture_diffuse, TexCoord).rgb;
	gAlbedoSpec.a = texture(texture_specular, TexCoord).r;
}