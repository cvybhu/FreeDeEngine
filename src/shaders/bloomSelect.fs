#version 330 core

uniform sampler2D screenTex;    //4
uniform float bloomMinBrightness;

out vec4 fragColor;

void main()
{
    vec2 texCoords = (gl_FragCoord.xy - vec2(0.5))/textureSize(screenTex, 0);

    vec3 color = texture(screenTex, texCoords).rgb; 

    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > bloomMinBrightness)
        fragColor = vec4(color, 1);
    else
        fragColor = vec4(0, 0, 0, 1);
}