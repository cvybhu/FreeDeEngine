#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoordsIn;

layout (std140) uniform posData
{
    mat4 view;
    mat4 projection;
    mat4 projView;
    mat4 dirLightSpace;
    vec3 viewPos;
};

out vec2 texCoords;

void main()
{
    texCoords = texCoordsIn;
    gl_Position = vec4(pos.x, pos.y, 0, 1);
}