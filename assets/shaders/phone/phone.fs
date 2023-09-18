#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture_diffuse0;

void main()
{
    FragColor = texture(texture_diffuse0, TexCoords);
}