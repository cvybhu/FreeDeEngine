#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoordsIn;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
    texCoords = texCoordsIn;
}
