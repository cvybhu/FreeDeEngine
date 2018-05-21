#version 330 core

out vec4 fagColor;

in vec3 color;

void main()
{
    fagColor = vec4(color, 1);
}