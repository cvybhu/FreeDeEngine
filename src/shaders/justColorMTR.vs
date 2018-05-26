#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 texCoordsIn;

uniform mat4 model;
uniform mat4 projView;

void main()
{
    gl_Position = projView * model * vec4(pos, 1.0);
}
