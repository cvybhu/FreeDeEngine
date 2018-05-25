#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 texCoordsIn;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT
{
    vec3 fragPos;
    vec2 texCoords;
    mat3 TBN;
    vec3 fragPosTan;
    vec3 viewPosTan;
    vec3 viewPos;
    //vec4 fragPosDirLightSpace;
} Out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 dirLightSpace;

uniform vec3 viewPos;

void main()
{
    mat3 normModel = mat3(transpose(inverse(model)));

    Out.fragPos = vec3(model * vec4(pos, 1.0));
    //Out.normal = normModel * normalIn;
    Out.texCoords = texCoordsIn;

    //Out.fragPosDirLightSpace = dirLightSpace * vec4(Out.fragPos, 1);

    vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
    vec3 N = normalize(vec3(model * vec4(normalIn,   0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent,   0.0)));

    mat3 TBN = mat3(T, B, N);

    Out.TBN = transpose(TBN);

    Out.fragPosTan = Out.TBN * Out.fragPos;
    Out.viewPosTan = Out.TBN * viewPos;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}

