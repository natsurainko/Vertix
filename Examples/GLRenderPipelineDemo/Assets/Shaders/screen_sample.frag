#version 450 core

in vec2 TexCoord;

layout (location = 0) out vec4 fColor;
layout (binding = 0) uniform sampler2D texture0;

void main() {
	fColor = texture(texture0, TexCoord);
}