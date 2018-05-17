#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 texCoordsIn;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{



    fragPos = vec3(model * vec4(pos, 1.0));
    normal =  mat3(transpose(inverse(model))) * normalIn;
    texCoords = texCoordsIn;

    gl_Position = projection * view * model * vec4(pos, 1.0);
    //vec3 pos = (gl_Position / gl_Position.w).xyz;
    //pos += normalIn;
    //gl_Position = vec4(pos, 1);
}

