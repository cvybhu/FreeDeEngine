#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 bloomColor;

uniform vec3 color;

layout (std140) uniform lightData
{
    vec3 ambientLight; float _padding; //4f
    int pointLightsNum;
    int shadowPointLightsNum;
    int isDirShadowActive;
    float bloomMinBright; //4f?

    /*
    PointLight pointLights[MAX_POINT_LIGHTS_NUM]; //n*4f
    PointLight shadowPointLights[MAX_SHADOW_POINT_LIGHTS]; //n*4f
    DirLight dirLight; //4f
    float shadowPointFarPlanes[MAX_SHADOW_POINT_LIGHTS]; //n*f (!)
    */
};

void main()
{
    FragColor = vec4(color, 1.0);
    
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > bloomMinBright)
        bloomColor = FragColor;
    else
        bloomColor = vec4(vec3(0), 1);
}

