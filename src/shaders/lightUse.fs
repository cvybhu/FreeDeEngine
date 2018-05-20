#version 330 core

struct PointLight {
    vec3 pos;

    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS_NUM 5


struct DirLight
{
    vec3 color;
    vec3 dir;
};


in VS_OUT
{
    vec3 fragPos;
    vec2 texCoords;
    vec4 fragPosDirLightSpace;
    vec3 T;
    vec3 N;
    vec3 B;
    mat3 TBN;
} In;


out vec4 fragColor;


uniform sampler2D diffTexture;
uniform sampler2D specTexture;
uniform sampler2D normalTexture;


uniform vec3 ambientLight;
uniform vec3 viewPos;

uniform PointLight pointLights[MAX_POINT_LIGHTS_NUM];
uniform int pointLightsNum;

uniform DirLight dirLight;

uniform sampler2D dirLightShadow;

uniform PointLight shadowPointLight;
uniform samplerCube shadowPointDepth;
uniform float shadowPLFarPlane;


float shininess = 32.0f;
float specularity = 0.2f;

vec3 calculatePointLight(PointLight light, vec3 normal)
{
    vec3 lightDir = normalize(light.pos - In.fragPos);
    vec3 viewDir = normalize(viewPos -In.fragPos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffTexture, In.texCoords).rgb;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

    vec3 specular = light.color * spec * specularity * texture(specTexture, In.texCoords).rgb;

    // attenuation
    float distance    = length(light.pos -In.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (diffuse + specular)*attenuation;
}


vec3 calculateDirLight(DirLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.dir);
    vec3 viewDir = normalize(viewPos - In.fragPos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffTexture, In.texCoords).rgb;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);

    vec3 specular = light.color * spec * specularity * texture(specTexture, In.texCoords).rgb;

    return (diffuse + specular);
}

/*
vec3 calcPointLights()
{
    vec3 result = vec3(0);

    for(int l = 0; l < MAX_POINT_LIGHTS_NUM; l++)   //<- this type of loop should work on old hw cause it can unroll it //TOCHECK
    {
        if(pointLightsNum == l)
            break;

        result += calculatePointLight(pointLights[l]);
    }

    return result;
}
*/


float dirLightShadowFact()
{
    // perform perspective divide
    vec3 projCoords = In.fragPosDirLightSpace.xyz / In.fragPosDirLightSpace.w;

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

float PointShadowCalculation()
{
    // get vector between fragment position and light position
    vec3 fragToLight =In.fragPos - shadowPointLight.pos;
    // use the light to fragment vector to sample from the depth map
    float closestDepth = texture(shadowPointDepth, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= shadowPLFarPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.03;
    //float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    vec3 sampleOffsetDirections[20] = vec3[]
    (
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    float shadow = 0.0;
    int samples  = 20;
    float viewDistance = length(viewPos -In.fragPos);
    float diskRadius = 0.05;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowPointDepth, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= shadowPLFarPlane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

float diffVec(vec3 a, vec3 b)
{
    return abs(a.x-b.x) + abs(a.y-b.y) + abs(a.z-b.z);
}


void main()
{
    if(texture(diffTexture, In.texCoords).a < 0.01)
        discard;

    vec3 result = ambientLight * texture(diffTexture, In.texCoords).rgb;

    vec3 normal = texture(normalTexture, In.texCoords).rgb;
    normal = normalize(normal * 2.0 - vec3(1.0));
    normal = (In.TBN * normal); 

    result += (1.0 - PointShadowCalculation()) * calculatePointLight(shadowPointLight, normal);
    result += (1.0 - dirLightShadowFact()) * calculateDirLight(dirLight, normal);
    fragColor = vec4(result, 1);
}
