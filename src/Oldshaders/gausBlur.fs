#version 330 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D prevTex;
uniform bool isHorizontal;

uniform float gaus[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec3 res = vec3(0);

    vec2 onePixel = 1.0 / textureSize(prevTex, 0);

    if(isHorizontal)
    {
        for(int i = 0; i < 5; i++)
        {
            res += gaus[i] * texture(prevTex, texCoords + vec2(onePixel.x, 0) * i).rgb;
            res += gaus[i] * texture(prevTex, texCoords - vec2(onePixel.x, 0) * i).rgb;
        }
    }   
    else
    {   
        for(int i = 0; i < 5; i++)
        {
            res += gaus[i] * texture(prevTex, texCoords + vec2(0, onePixel.y) * i).rgb;
            res += gaus[i] * texture(prevTex, texCoords - vec2(0, onePixel.y) * i).rgb;
        }
    }

    fragColor = vec4(res, 1);
}

