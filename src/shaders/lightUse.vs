#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 texCoordsIn;
layout (location = 3) in vec3 tangent;


out VS_OUT
{
    vec3 fragPos;
    vec2 texCoords;
    vec4 fragPosDirLightSpace;
    vec3 T;
    vec3 N;
    vec3 B;
    mat3 TBN;
} Out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 dirLightSpace;

void main()
{
    mat3 normModel = mat3(transpose(inverse(model)));

    Out.fragPos = vec3(model * vec4(pos, 1.0));
    //Out.normal = normModel * normalIn;
    Out.texCoords = texCoordsIn;

    Out.fragPosDirLightSpace = dirLightSpace * vec4(Out.fragPos, 1);

    vec3 T = normalize(normModel * tangent);
    vec3 N = normalize(normModel * normalIn);
    vec3 B = cross(N, T);

    mat3 TBN = mat3(T, B, N);

    Out.T = T;
    Out.N = N;
    Out.B = B;
    Out.TBN = TBN;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}

