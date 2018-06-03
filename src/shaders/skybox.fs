
#version 330 core
layout (location = 0) out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D normalAmbientOcc; //2
uniform samplerCube skybox; //7

uniform float exposure;

void main()
{
    vec2 texCoords = (gl_FragCoord.xy - vec2(0.5))/textureSize(normalAmbientOcc, 0);

    if(texture(normalAmbientOcc, texCoords).rgb != vec3(0))
        discard;

    FragColor = texture(skybox, TexCoords, 0).rgba;

    //exposure corection lol
    //FragColor = log(1 - FragColor) / -exposure;
}
