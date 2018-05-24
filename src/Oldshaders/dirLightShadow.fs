#version 330 core

in vec2 texCoords;

uniform sampler2D diffTex;


void main()
{
    if(texture(diffTex, texCoords).a < 0.1)discard;

    gl_FragDepth = gl_FragCoord.z;
}
