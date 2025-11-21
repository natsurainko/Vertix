#version 450 core

layout (location = 0) in vec2 vPosition;
layout (location = 1) in float vZIndex;
layout (location = 2) in vec4 vColor;

layout (location = 3) in vec2 vPositionTransform;
layout (location = 4) in vec2 vSizeTransform;

uniform bool isInstance = false;
uniform float zIndexInv = 1.0 / 1024.0;
uniform vec2 screenSizeInv;

out vec4 FragColor;

void main()
{
	vec2 pos = vPosition;

	if (isInstance) {
		pos = (vPositionTransform * 2.0 + vSizeTransform * (pos + 1.0)) * screenSizeInv - 1.0;
	}

	gl_Position = vec4(pos.x, -pos.y, vZIndex * zIndexInv, 1.0);
	FragColor = vColor;
}