#version 450 core

in vec4 FragColor;
in vec2 TexCoord;

layout (binding = 0) uniform sampler2D texture0;
layout (location = 0) out vec4 fColor;

void main()
{
    vec4 texColor = texture(texture0, TexCoord);
    fColor = FragColor * texColor;
}