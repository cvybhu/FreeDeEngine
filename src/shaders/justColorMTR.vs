#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 texCoordsIn;

uniform mat4 model;

layout (std140) uniform posData
{
    mat4 view;
    mat4 projection;
    mat4 projView;
    mat4 dirLightSpace;
    vec3 viewPos;
};

void main()
{
    gl_Position = projView * model * vec4(pos, 1.0);
}
