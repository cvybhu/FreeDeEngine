#version 330 core

#define MAX_POINT_LIGHTS_NUM 3
#define MAX_SHADOW_POINT_LIGHTS 2

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


in VS_OUT
{
    vec3 fragPos;
    vec2 texCoords;
    mat3 TBN;
    vec3 fragPosTan;
    vec3 viewPosTan;
    vec3 viewPos;
    vec4 fragPosDirLightSpace;
} In;


layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;


uniform sampler2D diffTexture;
uniform sampler2D specTexture;
uniform sampler2D normalTexture;
uniform sampler2D dispTex;
uniform sampler2D ambientOccTex;

uniform vec3 ambientLight;
uniform vec3 viewPos;

uniform PointLight pointLights[MAX_POINT_LIGHTS_NUM];
uniform int pointLightsNum;

uniform PointLight shadowPointLights[MAX_SHADOW_POINT_LIGHTS];
uniform samplerCube shadowPointDepth[MAX_SHADOW_POINT_LIGHTS];
uniform float shadowPointFarPlanes[MAX_SHADOW_POINT_LIGHTS];
uniform int shadowPointLightsNum;


uniform DirLight dirLight;
uniform sampler2D dirLightShadow;
uniform int isDirShadowActive;

uniform float bloomMinBright;


float shininess = 32.0f;
float specularity = 0.2f;


vec3 calculatePointLight(PointLight light, vec3 normal, vec2 texCoords)
{
    vec3 lightPos = In.TBN * light.pos;

    vec3 lightDir = normalize(lightPos - In.fragPosTan);
    vec3 viewDir = normalize(In.viewPosTan - In.fragPosTan);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffTexture, texCoords).rgb;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

    vec3 specular = light.color * spec * specularity * texture(specTexture, texCoords).rgb;

    // attenuation
    float distance    = length(lightPos - In.fragPosTan);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (diffuse + specular) * attenuation;
}


float PointShadowFact(PointLight light, samplerCube depthMap, float farPlane)
{
    // get vector between fragment position and light position
    vec3 fragToLight = In.fragPos - light.pos;
    // use the light to fragment vector to sample from the depth map
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= farPlane; //farPlane
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.3;
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
    float viewDistance = length(viewPos - In.fragPos);
    float diskRadius = 0.05;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farPlane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}


vec3 calcPointLights(vec3 normal, vec2 texCoords)
{
    vec3 result = vec3(0);

    for(int l = 0; l < MAX_POINT_LIGHTS_NUM; l++)   //<- this type of loop should work on old hw cause it can unroll it //TOCHECK
    {
        if(pointLightsNum == l)
            break;

        result += calculatePointLight(pointLights[l], normal, texCoords);
    }

    for(int l = 0; l < MAX_SHADOW_POINT_LIGHTS; l++)
    {
        if(shadowPointLightsNum == l)
            break;

        result += 
        (1.0 - PointShadowFact(shadowPointLights[l], shadowPointDepth[l], shadowPointFarPlanes[l]))
                         * calculatePointLight(shadowPointLights[l], normal, texCoords);
    }

    return result;
}






vec3 calculateDirLight(DirLight light, vec3 normal, vec2 texCoords)
{
    vec3 lightDir = normalize(In.TBN * - light.dir);
    vec3 viewDir = normalize(In.viewPosTan - In.fragPosTan);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffTexture, texCoords).rgb;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);

    vec3 specular = light.color * spec * specularity * texture(specTexture, texCoords).rgb;

    return (diffuse + specular);
}




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


vec2 parallaxMaping()
{
    float depth = 0.04;

    //float maxSteps = 10.f;
    //float minSteps = 5.f;
    //int(mix(maxSteps, minSteps, abs(dot(vec3(0.0, 0.0, -1.0), viewDir)) * 0.05));  

    int steps = 10;
    
    bool binsearch = true;
    int binsearchSteps = 10;
    float binsearchPrecision = 1.0 / pow(2.0, binsearchSteps);


    vec3 viewDir = normalize(In.fragPosTan - In.viewPosTan); 
    vec3 oneStep = viewDir / -viewDir.z * depth / float(steps);
    oneStep.z = -oneStep.z;

    vec2 res = In.texCoords;

    vec3 curVec = vec3(0);
    for(int s = 0; s <= steps; s++)
    {
        float curHeight = texture(dispTex, In.texCoords + curVec.xy).r * depth;

        if(curVec.z >= curHeight)
        {
            if(s != 0)
            {
                vec3 lastVec = curVec - oneStep;

                if(binsearch)
                {   
                    float low = 0;
                    float high = 1;
                    float res = 1;

                    //while(high - low > binsearchPrecision)
                    for(int bs = 0; bs < binsearchSteps; bs++)
                    {
                        float mid = (low + high)/2;

                        curVec = lastVec + mid * oneStep;

                        if(curVec.z >= texture(dispTex, In.texCoords + curVec.xy).r * depth)
                        {
                            res = mid;
                            high = mid - binsearchPrecision;
                        }
                        else
                            low = mid + binsearchPrecision;
                    }

                    curVec = lastVec + oneStep * res;
                }
                else
                {
                    float lastHeight = texture(dispTex, In.texCoords + lastVec.xy).r * depth;

                    float factor = (lastHeight - lastVec.z) / ((curVec.z - curHeight) + (lastHeight - lastVec.z));

                    curVec = lastVec + oneStep * factor;
                }
            }

            res = res + curVec.xy ;

            break;
        }

        curVec += oneStep;
    }

    return res;
}


bool parallaxMap = true;


void main()
{
    vec2 texCoords = In.texCoords;

    if(parallaxMap)
         texCoords = parallaxMaping();

    //if(texture(diffTexture, texCoords).a < 0.01)
        //discard;

    vec3 result = ambientLight * texture(diffTexture, texCoords).rgb;

    vec3 normal = texture(normalTexture, texCoords).rgb;
    normal = normalize(normal * 2.0 - vec3(1.0));
    //normal = (In.TBN * normal);

    result += calcPointLights(normal, texCoords);


    if(isDirShadowActive == 1)
        result += (1.0 - dirLightShadowFact()) * calculateDirLight(dirLight, normal, texCoords);
    else
        result += calculateDirLight(dirLight, normal, texCoords);

    fragColor = vec4(result, 1);

    fragColor *= texture(ambientOccTex, texCoords).r;

    
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > bloomMinBright)
        bloomColor = fragColor;
    else
        bloomColor = vec4(0, 0, 0, 1);
    

    //fragColor = vec4(vec3(texture(dirLightShadow, texCoords).r), 1);
//    fragColor = vec4(vec3(PointShadowFact(shadowPointLights[0], shadowPointDepth[0], 100.f)), 1);
}
