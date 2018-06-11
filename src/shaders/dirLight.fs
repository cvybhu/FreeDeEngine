#version 330 core

out vec4 fragColor;

uniform sampler2D albedoMetal;      //0
uniform sampler2D posRoughness;     //1
uniform sampler2D normalAmbientOcc; //2
//emmision?

layout (std140) uniform posData
{
    mat4 view;
    mat4 projection;
    mat4 projView;
    mat4 dirLightSpace;
    vec3 viewPos;
};

uniform vec3 lightDir;
uniform vec3 lightColor;

#define PI 3.14159265359

vec3 fresnelSchlick(float cosTheta, vec3 F0)    //[F]resnel equation - reflectivity on angle of material with base reflectivity F0
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)   // Normal [D]istribution - how many microfacets are aligned to halfway vector
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
    float k = (r*r) / 8.0; //<-for direct lighting

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) //[G]eometry function - takes selfshadowing of rough surfaces into account
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


void main()
{   
    vec2 texCoords = (gl_FragCoord.xy - vec2(0.5))/textureSize(albedoMetal, 0);
    
    vec3 albedo = texture(albedoMetal, texCoords).rgb;
    vec3 pos = texture(posRoughness, texCoords).rgb;
    vec3 normal = texture(normalAmbientOcc, texCoords).rgb;
    float metallic = texture(albedoMetal, texCoords).a;
    float roughness = texture(posRoughness, texCoords).a;
    float ambientOcc = texture(normalAmbientOcc, texCoords).a;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    if(normal == vec3(0))
        discard;

    vec3 radiance = lightColor;
    
    vec3 viewDir = normalize(viewPos - pos);
    vec3 lightVec = normalize(-lightDir);
    vec3 halfVec = normalize(viewDir + lightVec);

    //BRDF
    vec3 F  = fresnelSchlick(max(dot(halfVec, viewDir), 0.0), F0);
    float NDF = DistributionGGX(normal, halfVec, roughness);        
    float G   = GeometrySmith(normal, viewDir, lightVec, roughness);      
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightVec), 0.0);
    vec3 specular     = numerator / max(denominator, 0001) ;
            

    float NdotL = max(dot(normal, lightVec), 0.0);                
    vec3 res =  (kD * albedo / PI + specular) * radiance * NdotL; 

    fragColor = vec4(res, 1);

    //fragColor = vec4(vec3(dot(normal, viewDir)), 1);
}