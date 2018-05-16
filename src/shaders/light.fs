#version 330 core

struct PointLight {
    vec3 pos;

    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight
{
    vec3 color;

    vec3 dir;
};



out vec4 FragColor;

in vec3 fragPos;
in vec2 texCoords;
in vec3 normal;

uniform sampler2D diffTexture;
uniform sampler2D specTexture;


uniform vec3 ambientLight;
uniform vec3 viewPos;

uniform PointLight pointLights[20];
uniform int pointLightsNum;

uniform DirLight dirLights[20];
uniform int dirLightsNum;

float shininess = 32.0f;
float specularity = 0.5f;

vec3 calculatePointLight(PointLight light)
{

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
	attenuation = 1.0;

    return (diffuse + specular)*attenuation;
}


vec3 calculateDirLight(DirLight light)
{
    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.dir);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffTexture, texCoords).rgb;

    // specular
    vec3 viewDir = (viewPos - fragPos);

    float spec;

    bool blin = false;

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

    return (diffuse + specular);
}


void main()
{
    if(texture(diffTexture, texCoords).a < 0.01)
        discard;

    vec3 result = ambientLight * texture(diffTexture, texCoords).rgb;

    for(int i = 0; i < pointLightsNum; i++)
        result += calculatePointLight(pointLights[i]);

    for(int i = 0; i < dirLightsNum; i++)
        result += calculateDirLight(dirLights[i]);

    FragColor = vec4(result, 1.0);
}
