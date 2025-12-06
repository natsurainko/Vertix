#version 450 core

in vec4 FragColor;
in vec2 TexCoord;
in vec4 TextureRegion;

layout (binding = 0) uniform sampler2D texture0;
layout (location = 0) out vec4 fColor;

uniform float pxRange = 16.0;

void main()
{
    vec2 regionTexCoord = TextureRegion.xy + TexCoord * TextureRegion.zw;
    float distance = texture(texture0, regionTexCoord).r;

    vec2 unitRange = vec2(pxRange) / vec2(textureSize(texture0, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(regionTexCoord);
    float screenPxRange = max(0.5 * dot(unitRange, screenTexSize), 1.0);
    
    float screenPxDistance = screenPxRange * (distance - 0.5);
    float alpha = clamp(screenPxDistance + 0.7, 0.0, 1.0);
    
    fColor = vec4(FragColor.rgb, FragColor.a * alpha);
}