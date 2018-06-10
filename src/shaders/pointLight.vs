#version 330 core

layout (location = 0) in vec3 pos;


uniform vec3 lightPos;
uniform vec3 lightColor;

layout (std140) uniform posData
{
    mat4 view;
    mat4 projection;
    mat4 projView;
    mat4 dirLightSpace;
    vec3 viewPos;
};


float calculateRadius()
{
    float maxVal = max(lightColor.x, max(lightColor.y, lightColor.z));

    if(maxVal == 0)
        return 0;

    float minValInteresting = 0.005;

    return sqrt(maxVal/minValInteresting);
}

void main()
{
    float radius = calculateRadius();

    gl_Position = projView * vec4(pos * radius + lightPos, 1);
}