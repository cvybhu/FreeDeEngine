
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 bloomBackGround;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform float exposure;

void main()
{
    FragColor = texture(skybox, TexCoords);

    //exposure corection lol
    FragColor = log(1 - FragColor) / -exposure;
    bloomBackGround = vec4(0, 0, 0, 1);
}
