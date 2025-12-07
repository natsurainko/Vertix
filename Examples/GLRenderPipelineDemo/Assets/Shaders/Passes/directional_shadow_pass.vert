#version 450 core

layout (location = 0) in vec3 vPosition;

uniform mat4 world;
uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = lightSpaceMatrix * world * vec4(vPosition, 1.0);
}