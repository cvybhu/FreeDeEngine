#pragma once

#include <Texture.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>


struct Mesh //holds vertex data and pointers to used textures from global storage. Loadable from file and on GPU
{
    void loadToRAM(const char* filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();

    Texture* diffTex;
    Texture* specTex;
    Texture* normalTex; 
    Texture* dispTex; //displacement (parallax occlusion)
    Texture* ambientOccTex;

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };


    std::vector<Vertex> verts;
    int vertsNum;
    GLuint VBO, VAO;

    void calculateTangents();
};