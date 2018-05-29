#version 330 core

layout (location = 0) out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D albedoMetal;
uniform sampler2D posRoughness;
uniform sampler2D normalAmbientOcc;

struct DirLight
{
    vec3 dir;
    vec3 color;
};

struct PointLight
{
    vec3 pos;
    vec3 color;
};

uniform DirLight dirLight;
const int pointLightsNum = 2;
uniform PointLight pointLights[2]; //<- all this is gonna be done in other deffered way



layout (std140) uniform posData
{
    mat4 view;
    mat4 projection;
    mat4 projView;
    mat4 dirLightSpace;
    vec3 viewPos;
};

layout (std140) uniform lightData //TODO
{
    vec3 ambientLight;
    float minBloomBrightness;
    float exposure;
    float gamma;
};


#define PI 3.14

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 calculateLight(vec3 radiance, vec3 lightDir)
{
    vec3 albedo = texture(albedoMetal, texCoords).rgb;
    vec3 pos = texture(posRoughness, texCoords).rgb;
    vec3 normal = texture(normalAmbientOcc, texCoords).rgb;

    float metallic = texture(albedoMetal, texCoords).a;
    float roughness = texture(posRoughness, texCoords).a;
    float ambientOcc = texture(normalAmbientOcc, texCoords).a;

    vec3 viewDir = normalize(viewPos - pos);
    vec3 half = normalize(viewDir + lightDir);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(half, viewDir), 0.0), F0);

        
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, half, roughness);        
    float G   = GeometrySmith(normal, viewDir, lightDir, roughness);      
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);  
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, lightDir), 0.0);                
    return (kD * albedo / PI + specular) * radiance * NdotL; 
}


float calcAttentuation(vec3 position)
{
    vec3 pos = texture(posRoughness, texCoords).rgb;
    float distance = length(position - pos);
    return 1.f / (distance * distance);
}

void main()
{
    vec3 normal = texture(normalAmbientOcc, texCoords).rgb;
    if(normal == vec3(0))
        discard;

    //float distance    = length(lightPositions[i] - WorldPos);
    //float attenuation = 1.0 / (distance * distance);
    //vec3 radiance     = lightColors[i] * attenuation;        
    vec3 pos = texture(posRoughness, texCoords).rgb;
    
    vec3 res = vec3(0);
    res  += calculateLight(dirLight.color, -dirLight.dir);
    res += calculateLight(calcAttentuation(pointLights[0].pos) * pointLights[0].color, normalize(pointLights[0].pos - pos));
    res += calculateLight(calcAttentuation(pointLights[1].pos) * pointLights[1].color, normalize(pointLights[1].pos - pos));

    fragColor = vec4(res, 1);
}