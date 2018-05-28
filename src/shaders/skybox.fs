
#version 330 core
layout (location = 0) out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform float exposure;

void main()
{
    FragColor = texture(skybox, TexCoords).rgba;

    //exposure corection lol
    FragColor = log(1 - FragColor) / -exposure;
}
