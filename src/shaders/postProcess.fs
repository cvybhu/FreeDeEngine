#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTex;
uniform sampler2D bloomTex;


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

uniform float exposure;


void main()
{
    vec3 effectColor = vec3(0.00);

    for(int i = 0; i < 9; i++)
        effectColor += edgeDetectKernel[i]/3.0*vec3(texture(screenTex, texCoords.st + moves[i]));

    //float mixFactor = min(max(1.1-length(texCoords - vec2(0.5))*0.2, 0), 1);
    //FragColor.xyz = mix(effectColor*50, texture(screenTex, texCoords).rgb, mixFactor);

    FragColor = vec4(texture(screenTex, texCoords).rgb, 1);

    //reinhard hdr tone mapping
    //FragColor = FragColor / (FragColor + 1.0);

    //FragColor += vec4(texture(bloomTex, texCoords).rgb, 0);

    //exposure hdr tone mapping
    FragColor = 1 - exp(-FragColor * exposure);


    //gamma correction
    float gamma = 2.2;
    FragColor.xyz = pow(FragColor.xyz, vec3(1.0/gamma));

    //FragColor = vec4(texture(screenTex, texCoords).rg, 0, 1);
}
