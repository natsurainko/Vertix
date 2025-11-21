#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 3) in mat4 instanceModel;

uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    FragPos = vec3(instanceModel * vec4(vPosition, 1.0));
    Normal = mat3(transpose(inverse(instanceModel))) * vNormal;
    gl_Position = projection * view * instanceModel * vec4(vPosition, 1.0);
}