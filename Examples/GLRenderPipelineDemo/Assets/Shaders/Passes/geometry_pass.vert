#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

// layout (location = 3) in vec4 vTextureRegion;
// layout (location = 4) in mat4 instanceMatrix;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

out vec4 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    vec4 worldPos = world * vec4(vPosition, 1.0);
    vec4 viewPos = view * worldPos;

    FragPos.xyz = worldPos.xyz;
    FragPos.w = -viewPos.z;

    Normal = mat3(transpose(inverse(world))) * vNormal;
    TexCoord = vTexCoord;

    gl_Position = projection * viewPos;
}