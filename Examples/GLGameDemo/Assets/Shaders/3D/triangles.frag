#version 450 core

in vec3 Normal;
in vec3 FragPos;

layout (location = 0) out vec4 fColor;

const vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 objectColor = vec3(0.5, 0.4, 0.8);

void main()
{
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = 0.3 * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    fColor = vec4(result, 1.0);
}