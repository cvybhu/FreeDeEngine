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

    Texture* albedoTex = nullptr;
    Texture* metallicTex = nullptr; 
    Texture* roughnessTex = nullptr;
    Texture* normalTex = nullptr;
    Texture* ambientOccTex = nullptr;
    Texture* displacementTex = nullptr;
    Texture* emmisionTex = nullptr;

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