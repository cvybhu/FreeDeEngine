#version 330 core

struct PointLight {
    vec3 pos;

    vec3 color;

    float constant;
    float linear;
    float quadratic;
};



out vec4 FragColor;

in vec3 fragPos;
in vec2 texCoords;
in vec3 normal;

uniform sampler2D diffTexture;
uniform sampler2D specTexture;


uniform vec3 ambientLight;
uniform vec3 viewPos;

uniform PointLight pointLights[10];
uniform int pointLightsNum;


vec3 calculatePointLight(PointLight light)
{
    float shininess = 32.0f;
    float specularity = 0.5f;

    // ambient

    vec3 ambient = ambientLight * texture(diffTexture, texCoords).rgb;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffTexture, texCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);

    float spec;

    bool blin = true;

    if(blin)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }

    vec3 specular = light.color * spec * specularity * texture(specTexture, texCoords).rgb;

    // attenuation
    float distance    = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular)*attenuation;
}





void main()
{
    vec3 result = vec3(0, 0, 0);

    for(int i = 0; i < pointLightsNum; i++)
        result += calculatePointLight(pointLights[i]);

    FragColor = vec4(result, 1.0);
}
