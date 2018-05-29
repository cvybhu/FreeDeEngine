#version 330 core

in VS_OUT
{
    vec3 fragPos;
    vec2 texCoords;
    mat3 TBN;
    //vec3 fragPosTan;
    //vec3 viewPosTan;
} In;

uniform sampler2D albedoTex;      //0
uniform sampler2D metallicTex;    //1
uniform sampler2D roughnessTex;   //2
uniform sampler2D normalTex;      //3
uniform sampler2D ambientOccTex;  //4
uniform sampler2D displacementTex;//5
uniform sampler2D emmisionTex;    //6


layout (location = 0) out vec4 albedoMetal;
layout (location = 1) out vec4 posRoughness;
layout (location = 2) out vec4 normalAmbientOcc;

void main()
{
    vec2 texCoords = In.texCoords;

    albedoMetal.rgb = texture(albedoTex, texCoords).rgb;
    albedoMetal.a = texture(metallicTex, texCoords).r;

    posRoughness.rgb = In.fragPos;
    posRoughness.a = texture(roughnessTex, texCoords).r;

    vec3 normal = In.TBN * vec3(0, 0, 1);//texture(normalTex, texCoords).rgb;

    normalAmbientOcc.rgb = normalize(normal);
    normalAmbientOcc.a = texture(ambientOccTex, texCoords).r;
}