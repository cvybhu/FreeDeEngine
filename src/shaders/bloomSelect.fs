#version 330 core

uniform sampler2D screenTex;    //4
uniform float bloomMinBrightness;

out vec4 fragColor;

void main()
{
    vec2 texCoords = (gl_FragCoord.xy - vec2(0.5))/textureSize(screenTex, 0);

    vec3 color = texture(screenTex, texCoords).rgb; 
    fragColor = vec4(color, 1);
}