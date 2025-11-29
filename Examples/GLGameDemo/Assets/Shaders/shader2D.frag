#version 450 core

in vec4 FragColor;
in vec2 TexCoord;
in vec4 TextureRegion;

layout (binding = 0) uniform sampler2D texture0;
layout (location = 0) out vec4 fColor;

void main()
{
    vec2 regionTexCoord = TextureRegion.xy + TexCoord * TextureRegion.zw;
    fColor = FragColor * texture(texture0, regionTexCoord);
}