#version 330 core

#define MAX_POINT_LIGHTS_NUM 10
#define MAX_SHADOW_POINT_LIGHTS 2

struct PointLight {
    vec3 pos;
    vec3 color;
};


struct DirLight{
    vec3 color;
    vec3 dir;
};


in VS_OUT
{
    vec3 fragPos;
    vec2 texCoords;
    mat3 TBN;
    //vec3 fragPosTan;
    //vec3 viewPosTan;
} In;

layout (std140) uniform posData
{
    mat4 view;
    mat4 projection;
    mat4 projView;
    mat4 dirLightSpace;
    vec3 viewPos;
};

layout (std140) uniform lightData
{
    vec3 ambientLight; float _padding; //4f
    
    DirLight dirLight; //4f

    PointLight pointLights[MAX_POINT_LIGHTS_NUM]; //n*4f
    int pointLightsNum; 

    float bloomMinBright;
    float exposure;
};

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D metallicTex;
uniform sampler2D roughnessTex;
uniform sampler2D ambientOccTex;


void main()
{





}