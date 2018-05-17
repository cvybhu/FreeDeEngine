#version 330 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D theTexture;


void main()
{
    fragColor = vec4(texture(theTexture, texCoords).rgb, 1);
}
