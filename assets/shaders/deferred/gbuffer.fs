#version 330 core
layout (location = 0) out vec4 gAlbedoM;
layout (location = 1) out vec4 gNormalR;
layout (location = 2) out vec4 gPositionA;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;


// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;


vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


void main()
{    

    gPositionA.xyz = WorldPos;
    gPositionA.z = texture(aoMap, TexCoords).r;

    gNormalR.rgb = normalize(Normal);//getNormalFromMap();
    gNormalR.a = texture(roughnessMap, TexCoords).r;

    gAlbedoM.rgb = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));

    gAlbedoM.a = texture(metallicMap, TexCoords).r;
}