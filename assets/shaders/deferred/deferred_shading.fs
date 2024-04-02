#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D gAlbedoM;
uniform sampler2D gNormalR;
uniform sampler2D gPositionA;
uniform sampler2D ssao;


// IBL
uniform bool IBL;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;


struct Light {
    vec3 position;
    vec3 color;
    
    float linear;
    float quadratic;
    float radius;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform vec3 camPos;
uniform float farPlane;

const float PI = 3.14159265359;


// ----------------------------------------------------------------------------
// 法线分布方程
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
// 几何遮蔽，计算光线在粗糙表面上被遮挡的比例
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
// 计算反射和折射的比例
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   


/*

// 计算阴影
float ShadowCalculationDir(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;

    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(dirShadowMap,0);
    vec3 normal = normalize(Normal);
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, dirLight.direction)), 0.005);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(dirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 0.111111 : 0.0;        
        }       
    }

    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    //float closestDepth = texture(dirShadowMap, projCoords.xy).r; 

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculationPoint(uint index,vec3 fragToLight,float viewDistance)
{

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 8;  // TODO 采样有问题
    float inv_samples = 1.0 / samples;
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
    float currentDepth = length(fragToLight) - bias;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMaps[index], fragToLight + gridSamplingDisk[i] , diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]
        if(currentDepth > closestDepth)
            shadow += inv_samples;
    }     
     
    return shadow;
    // float closestDepth = texture(pointShadowMaps[index], fragToLight).r;
    // closestDepth *= farPlane;
    // return currentDepth > closestDepth ? 1.0:0.0;
}


vec3 CalcDirLight(vec3 V, vec3 N, vec3 albedo, float metallic, float roughness,float shadow, vec3 F0)
{
    vec3 L = normalize(dirLight.direction);
    vec3 H = normalize(L + V);
    float NdotV = max(dot(N,V),0.0);
    float NdotL = max(dot(N,L),0.0);
    vec3 radianceIn = dirLight.color;

    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * NdotV * NdotL + 0.0001; 
    vec3 specular = numerator / denominator;

    vec3 radiance =  (kD * (albedo / PI) + specular) * radianceIn * NdotL;
    radiance *= (1.0 - shadow);

    return radiance;
}
*/

vec3 CalcPointLight(int index,vec3 worldPos,vec3 V,vec3 N,float roughness,float metallic,vec3 albedo,vec3 F0,float viewDistance)
{
    // light的直接光贡献
    vec3 lightPosition = lights[index].position;
    float distance = length(lightPosition - worldPos);
    if(distance > lights[index].radius) return vec3(0.0);


    vec3 L = normalize(lightPosition - worldPos);
    vec3 H = normalize(V + L);
    float attenuation = 1.0 / (distance * distance);
    //float attenuation = 1.0 / (1.0 + 0.7*distance + 1.8 * distance * distance);
    vec3 radianceIn = lights[index].color * attenuation;
    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    // 基于Cook-Torrance BRDF计算间接光
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * NdotV * NdotL + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  

    vec3 radiance = (kD * albedo / PI + specular) * radianceIn * NdotL;  
    // 添加点光源阴影  
    // vec3 fragToLight = worldPos - lightPosition;
    // float shadow = ShadowCalculationPoint(index,fragToLight,viewDistance);
    // radiance *= (1.0 - shadow);

    return vec3(radiance);
}




void main(){
    vec4 albedoM     = texture(gAlbedoM, TexCoords).rgba;
    float ssao_val     = texture(ssao, TexCoords).r;
    vec3 albedo = albedoM.rgb * ssao_val;
    float metallic  = albedoM.a;

    vec4 normalR = texture(gNormalR, TexCoords).rgba;
    vec3 N = normalR.rgb;
    float roughness = normalR.a;

    vec4 positionA = texture(gPositionA, TexCoords).rgba;
    vec3 WorldPos = positionA.rgb;
    float ao        = positionA.a;

    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    float viewDistance = length(camPos - WorldPos);

    for(int i = 0; i < NR_LIGHTS; ++i) 
    {
        Lo += CalcPointLight(i,WorldPos,V,N,roughness,metallic,albedo,F0,viewDistance); 
    }  

    // 添加平行光阴影
    // float shadow = ShadowCalculationDir(FragPosLightSpace);
    // vec3 dirLo= CalcDirLight(V,N,albedo,metallic,roughness,shadow,F0);
    // Lo += dirLo;

    vec3 ambient = vec3(0.025) * albedo;
    // if(IBL){
    //     // ambient lighting (we now use IBL as the ambient term)
    //     vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    //     vec3 kS = F;
    //     vec3 kD = 1.0 - kS;
    //     kD *= 1.0 - metallic;	  

    //     vec3 irradiance = texture(irradianceMap, N).rgb;
    //     vec3 diffuse      = irradiance * albedo;

    //     // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    //     const float MAX_REFLECTION_LOD = 4.0;
    //     vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    //     vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    //     vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    //     ambient = (kD * diffuse + specular);
    // }
    ambient *= ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    // float exposure = 1.0; // 曝光度
    // color = vec3(1.0) - exp(-color * exposure);//color / (color + vec3(1.0));
    // gamma correct
    //color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(vec3(color) , 1.0);

    float brightness = dot(color,vec3(0.2126,0.7152,0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(color,1.0);
    else
        BrightColor = vec4(0.0,0.0,0.0,0.0);
}