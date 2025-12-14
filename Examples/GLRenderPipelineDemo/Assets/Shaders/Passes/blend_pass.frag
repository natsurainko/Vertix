#version 450 core

in vec2 TexCoord;

layout (location = 0) out vec4 fColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAlbedoSpec;
layout (binding = 3) uniform sampler2D gShadow;
layout (binding = 4) uniform sampler2D gAmbientOcclusion;

uniform vec3 viewPos;
uniform vec3 lightDirection;

float SampleAmbientOcclusion() {
    vec2 texelSize = 1.0 / vec2(textureSize(gAmbientOcclusion, 0));
    float result = 0.0;

    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(gAmbientOcclusion, TexCoord + offset).r;
        }
    }

    return result / 16.0;
}

void main() {
    vec4 fragPos = texture(gPosition, TexCoord);
    vec3 normal = texture(gNormal, TexCoord).rgb;
    vec3 color = texture(gAlbedoSpec, TexCoord).rgb;
    
    if (fragPos.a == 0.0) {
        discard;
        return;
    }

    vec3 lightColor = vec3(0.5);

    // ambient
    float ao = SampleAmbientOcclusion();
    vec3 ambient = 0.3 * color * ao;

    // diffuse
    float diff = max(dot(-lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos.rgb);
    vec3 reflectDir = reflect(lightDirection, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(-lightDirection + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // calculate shadow
    float shadow = texture(gShadow, TexCoord).r;                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    fColor = vec4(lighting, 1.0);
}