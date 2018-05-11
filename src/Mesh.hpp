#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

class Texture;

class Mesh
{
public:
    Mesh();

    void loadToRAM(const char* filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();


    Texture* myTexture;

    const GLuint& getVAO() const;

    int getVertsNum() const;

//ivate:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    std::vector<Vertex> verts;
    GLuint VBO, VAO;
};
