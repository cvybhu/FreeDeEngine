#version 330 core

out vec4 color;

uniform sampler2D prevTex;

void main()
{
    vec2 texCoords = (gl_FragCoord.xy - vec2(0.5))/textureSize(prevTex, 0);
    
    vec2 unit = vec2(1) / textureSize(prevTex, 0);
    
    color = texture(prevTex, 2*texCoords + unit);

    /*
    color = 
    (texture(prevTex, texCoords + vec2(0, 0)) +
    texture(prevTex, texCoords + vec2(unit.x, 0)) +
    texture(prevTex, texCoords + vec2(0, unit.y)) +
    texture(prevTex, texCoords + vec2(unit.x, unit.y)) );
    color /= 4;
    */
}
