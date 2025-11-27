#version 450 core

in vec4 FragColor;
in vec2 TexCoord;

layout (binding = 0) uniform sampler2D texture0;
layout (location = 0) out vec4 fColor;

uniform float pxRange = 16.0;

void main()
{
    float distance = texture(texture0, TexCoord).r;

    vec2 unitRange = vec2(pxRange) / vec2(textureSize(texture0, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(TexCoord);
    float screenPxRange = max(0.5 * dot(unitRange, screenTexSize), 1.0);
    
    float screenPxDistance = screenPxRange * (distance - 0.5);
    float alpha = clamp(screenPxDistance + 0.6, 0.0, 1.0);
    
    fColor = vec4(FragColor.rgb, FragColor.a * alpha);
}