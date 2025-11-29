#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec2 vTexCoord;

layout (location = 3) in vec4 vTextureRegion;
layout (location = 4) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec4 FragColor;
out vec2 TexCoord;
out vec4 TextureRegion;

void main()
{
    FragColor = vColor;
    TexCoord = vTexCoord;
    TextureRegion = vTextureRegion;

    gl_Position = projection * view * instanceMatrix * vec4(vPosition, 1.0);
}