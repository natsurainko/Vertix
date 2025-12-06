#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 window;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main()
{
    TexCoord = vTexCoord;
    gl_Position = projection * view * window * vec4(vPosition, 1.0);
}