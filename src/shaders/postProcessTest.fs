#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2DMS screenTexture;


float offset = 1.0 / 700.0;

vec2 moves[9] = vec2[](
vec2(-offset,  offset), // top-left
vec2( 0.0f,    offset), // top-center
vec2( offset,  offset), // top-right
vec2(-offset,  0.0f),   // center-left
vec2( 0.0f,    0.0f),   // center-center
vec2( offset,  0.0f),   // center-right
vec2(-offset, -offset), // bottom-left
vec2( 0.0f,   -offset), // bottom-center
vec2( offset, -offset)  // bottom-right
);

float edgeDetectKernel[9] = float[](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

float blurKernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
);



void main()
{
    /*
    vec3 res = vec3(0.00);

    for(int i = 0; i < 9; i++)
        res += blurKernel[i]/3.0*vec3(texture(screenTexture, texCoords.st + moves[i]));

    FragColor = vec4(res, 1);
    */

    vec2 texSize = textureSize(screenTexture);

    for(int i = 0; i < 4; i++)
        FragColor += texelFetch(screenTexture, ivec2(texCoords.x*texSize.x, texCoords.y*texSize.y), i);

    FragColor /= 4.0;

    float gamma = 2.2;

    FragColor.xyz = pow(FragColor.xyz, vec3(1.0/gamma));
}
