#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
    vec3 envColor = textureLod(environmentMap, WorldPos,1.2).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);

    float brightness = dot(envColor,vec3(0.2126,0.7152,0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(envColor,1.0);
    else
        BrightColor = vec4(0.0,0.0,0.0,0.0);
}