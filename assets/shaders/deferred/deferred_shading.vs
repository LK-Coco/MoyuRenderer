#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
    WorldPos = gl_Position.xyz;
}