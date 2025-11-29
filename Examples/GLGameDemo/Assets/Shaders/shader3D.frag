#version 450 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 TextureRegion;

layout (binding = 0) uniform sampler2D texture0;
layout (location = 0) out vec4 fColor;

uniform vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main()
{
    vec2 regionTexCoord = TextureRegion.xy + TexCoord * TextureRegion.zw;
    vec4 texColor = texture(texture0, regionTexCoord);

    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = 0.3 * lightColor;

    vec3 result = (ambient + diffuse);
    fColor = vec4(result, 1.0) * texColor;
}