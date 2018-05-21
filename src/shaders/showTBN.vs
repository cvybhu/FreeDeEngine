#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 texCoordsIn;
layout (location = 3) in vec3 tangentIn;
layout (location = 4) in vec3 bitangentIn;

out vec3 normal;
out vec3 tangent;
out vec3 bitangent;

uniform mat4 model;

void main()
{
    tangent = normalize(vec3(model * vec4(tangentIn,   0.0)));
    normal = normalize(vec3(model * vec4(normalIn,   0.0)));
    bitangent = normalize(vec3(model * vec4(bitangentIn,   0.0)));

    gl_Position = model * vec4(pos, 1);
}