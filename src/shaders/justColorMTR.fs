#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 bloomColor;

uniform vec3 color;
uniform float bloomMinBright;

void main()
{
    FragColor = vec4(color, 1.0);
    
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > bloomMinBright)
        bloomColor = FragColor;
    else
        bloomColor = vec4(vec3(0), 1);
}

