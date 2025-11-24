#version 450 core

in vec4 FragColor;
in vec2 TexCoord;

layout(binding = 0) uniform sampler2D texture0;
layout (location = 0) out vec4 fColor;

uniform float smoothing = 0.1;

void main()
{
    float distance = texture(texture0, TexCoord).r;
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
    fColor = vec4(FragColor.rgb, FragColor.a * alpha);
}