#version 330 core

layout (location = 0) out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D albedoMetal;
uniform sampler2D posRoughness;
uniform sampler2D normalAmbientOcc;

void main()
{
    vec3 albedo = texture(albedoMetal, texCoords).rgb;
    vec3 pos = texture(posRoughness, texCoords).rgb;
    vec3 normal = texture(normalAmbientOcc, texCoords).rgb;

    float metallic = texture(albedoMetal, texCoords).a;
    float roughness = texture(posRoughness, texCoords).a;
    float ambientOcc = texture(normalAmbientOcc, texCoords).a;

    fragColor = vec4(albedo, 1);
}