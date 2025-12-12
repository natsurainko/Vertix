#version 450 core

in vec2 TexCoord;

layout (location = 0) out vec4 fColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAlbedoSpec;
layout (binding = 3) uniform sampler2D gShadow;

uniform vec3 viewPos;
uniform vec3 lightDirection;

void main() {
	vec3 fragPos = texture(gPosition, TexCoord).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoord).rgb);
    vec3 color = texture(gAlbedoSpec, TexCoord).rgb;
    
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    float diff = max(dot(-lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
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