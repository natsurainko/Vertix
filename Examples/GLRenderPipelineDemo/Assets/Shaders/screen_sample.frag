#version 450 core

in vec2 TexCoord;

layout (location = 0) out vec4 fColor;
layout (binding = 0) uniform sampler2D texture0;

uniform bool isSingleValue;

void main() {
	if (isSingleValue) {
		fColor = vec4(vec3(texture(texture0, TexCoord).r), 1.0);
	} else {
		fColor = texture(texture0, TexCoord);
	}

	// fColor = vec4(vec3(texture(texture0, vec3(TexCoord, 4)).r), 1.0);
}