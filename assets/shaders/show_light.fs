#version 330 core
layout (location = 0) out vec4 FragColor;

uniform vec3 lightColor;

void main()
{           
    FragColor = vec4(lightColor/3, 1.0);
}