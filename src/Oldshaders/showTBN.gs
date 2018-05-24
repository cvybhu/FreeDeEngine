#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;


in vec3 normal[];
in vec3 tangent[];
in vec3 bitangent[];

uniform mat4 projView;

out vec3 color;


void main()
{
    float len = 0.5;
    
    vec3 midPos = (vec3(gl_in[0].gl_Position) + vec3(gl_in[1].gl_Position) + vec3(gl_in[2].gl_Position))/3.0;

    color = vec3(0, 0, 1);
    gl_Position = projView * vec4(midPos, 1);
    EmitVertex();
    gl_Position = projView * vec4(midPos + normal[0] * len, 1);
    EmitVertex();
    EndPrimitive();

    color = vec3(1, 0, 0);
    gl_Position = projView * vec4(midPos, 1);
    EmitVertex();
    gl_Position = projView * vec4(midPos + tangent[0] * len, 1);
    EmitVertex();
    EndPrimitive();

    color = vec3(0, 1, 0);
    gl_Position = projView * vec4(midPos, 1);
    EmitVertex();
    gl_Position = projView * vec4(midPos + bitangent[0] * len, 1);
    EmitVertex();
    EndPrimitive();
}