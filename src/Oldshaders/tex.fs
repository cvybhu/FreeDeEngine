#version 330 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;

uniform sampler2D theTexture;

void main()
{
    FragColor = texture(theTexture, texCoords);
}
