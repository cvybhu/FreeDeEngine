
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 texCoordsIn;

out vec2 texCoords;

uniform mat4 model;

void main()
{
    texCoords = texCoordsIn;
    gl_Position = model * vec4(aPos, 1.0);
}
