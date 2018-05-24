
#version 330 core
layout (location = 0) in vec3 pos;
//layout (location = 1) in vec3 normalIn;
//layout (location = 2) in vec2 texCoordsIn;
//layout (location = 3) in vec3 tangent;
//layout (location = 4) in vec3 bitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
