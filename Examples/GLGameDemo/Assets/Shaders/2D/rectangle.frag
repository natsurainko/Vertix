#version 450 core

in vec4 FragColor;

layout (location = 0) out vec4 fColor;

void main()
{
    fColor = FragColor;
}