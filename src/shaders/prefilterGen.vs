#version 330 core
layout(location = 0) in vec3 pos;

uniform mat4 projection;
uniform mat4 view;

out vec3 normal;

void main()
{
    normal = pos;
    gl_Position = projection * view * vec4(pos, 1);
}
