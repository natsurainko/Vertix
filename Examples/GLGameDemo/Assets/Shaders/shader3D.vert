#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 3) in vec4 vTextureRegion;
layout (location = 4) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 TextureRegion;

void main()
{
    FragPos = vec3(instanceMatrix * vec4(vPosition, 1.0));
    Normal = mat3(transpose(inverse(instanceMatrix))) * vNormal;
    TexCoord = vTexCoord;
    TextureRegion = vTextureRegion;

    gl_Position = projection * view * instanceMatrix * vec4(vPosition, 1.0);
}