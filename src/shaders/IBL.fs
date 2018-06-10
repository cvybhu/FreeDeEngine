#version 330 core

layout (location = 0) out vec4 fragColor;

uniform sampler2D albedoMetal;      //0
uniform sampler2D posRoughness;     //1
uniform sampler2D normalAmbientOcc; //2
//emmision?

uniform samplerCube envIrradiance;  //4
uniform samplerCube prefilterMap;   //5
uniform sampler2D brdfLUTTex;       //6

uniform samplerCube skybox; //7

layout (std140) uniform posData
{
    mat4 view;
    mat4 projection;
    mat4 projView;
    mat4 dirLightSpace;
    vec3 viewPos;
};



#define PI 3.14159265359

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)   //Modified approximated Fresnel for IBL purposes
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

void main()
{
    vec2 texCoords = (gl_FragCoord.xy - vec2(0.5))/textureSize(albedoMetal, 0);
    

    vec3 albedo = texture(albedoMetal, texCoords).rgb;
    vec3 pos = texture(posRoughness, texCoords).rgb;
    vec3 normal = texture(normalAmbientOcc, texCoords).rgb;
    float metallic = texture(albedoMetal, texCoords).a;
    float roughness = texture(posRoughness, texCoords).a;
    float ambientOcc = texture(normalAmbientOcc, texCoords).a;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 kS = fresnelSchlickRoughness(max(dot(normal, normalize(viewPos - pos)), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(envIrradiance, normal).rgb;
    vec3 diffuse = irradiance * albedo;
    

    vec3 V = normalize(viewPos - pos);
    vec3 N = normalize(normal);
    vec3 R = reflect(-V, N);   

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD) .rgb;

    vec3 F        = kS;//fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec2 envBRDF  = texture(brdfLUTTex, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    fragColor = vec4((kD * diffuse + specular) * ambientOcc, 1); 
}