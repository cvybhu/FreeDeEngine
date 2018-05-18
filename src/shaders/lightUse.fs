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


in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;


uniform sampler2D diffTexture;
uniform sampler2D specTexture;


uniform vec3 ambientLight;
uniform vec3 viewPos;

uniform PointLight pointLights[5];
uniform int pointLightsNum;

uniform DirLight dirLight;

uniform sampler2D dirLightShadow;

in vec4 fragPosDirLightSpace;



float shininess = 32.0f;
float specularity = 0.2f;

vec3 calculatePointLight(PointLight light)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffTexture, texCoords).rgb;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

    vec3 specular = light.color * spec * specularity * texture(specTexture, texCoords).rgb;

    // attenuation
    float distance    = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	attenuation = 1.0;

    return (diffuse + specular)*attenuation;
}


vec3 calculateDirLight(DirLight light)
{
    vec3 lightDir = normalize(-light.dir);
    vec3 viewDir = normalize(viewPos - fragPos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffTexture, texCoords).rgb;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);

    vec3 specular = light.color * spec * specularity * texture(specTexture, texCoords).rgb;

    return (diffuse + specular);
}

vec3 calcPointLights()
{
    vec3 result = vec3(0);

    if(pointLightsNum <= 0)return result; result += calculatePointLight(pointLights[0]);
    if(pointLightsNum <= 1)return result; result += calculatePointLight(pointLights[1]);
    if(pointLightsNum <= 2)return result; result += calculatePointLight(pointLights[2]);
    if(pointLightsNum <= 3)return result; result += calculatePointLight(pointLights[3]);
    if(pointLightsNum <= 4)return result; result += calculatePointLight(pointLights[4]);
    //if(pointLightsNum <= 5)return result; result += calculatePointLight(pointLights[5]);

    return result;
}


float dirLightShadowFact()
{
    // perform perspective divide
    vec3 projCoords = fragPosDirLightSpace.xyz / fragPosDirLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(dirLightShadow, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    float bias = 0.002;

    //return (currentDepth - bias > texture(dirLightShadow, projCoords.xy).r) ? 1 : 0;


    vec2 texelSize = 1.0 / textureSize(dirLightShadow, 0);
    float res = 0;
    for(int x = -1; x <= 1; x++)
    for(int y = -1; y <= 1; y++)
        if(currentDepth - bias > texture(dirLightShadow, projCoords.xy + vec2(x*texelSize.x, y*texelSize.y)).r)
            res += 1;

    res /= 9;

    return res;

}


void main()
{
    if(texture(diffTexture, texCoords).a < 0.01)
        discard;

    vec3 result = ambientLight * texture(diffTexture, texCoords).rgb;

    result += calcPointLights();

    result += (1.0 - dirLightShadowFact()) * calculateDirLight(dirLight);;

    FragColor = vec4(result, 1);
}
