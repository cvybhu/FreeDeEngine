#version 330 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

in vec2 texCoords;

uniform sampler2D theTexture;


void main()
{
    fragColor = vec4(texture(theTexture, texCoords).rgb, 1);
    bloomColor = vec4(0, 0, 0, 1);
}
