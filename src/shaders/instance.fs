#version 330 core

out vec4 fragColor;

in vec2 texCoords;

//uniform smapler2D theTexture;


void main()
{
    //fragColor = vec4(texture(theTexture).rgb, 1);
    fragColor = vec4(1);
}
