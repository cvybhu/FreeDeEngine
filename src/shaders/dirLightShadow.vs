#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 texCoordsIn;

out vec2 texCoords;

uniform mat4 lightTrans;
uniform mat4 model;

void main()
{
    gl_Position = lightTrans * model * vec4(pos, 1);

    texCoords = texCoordsIn;
}
