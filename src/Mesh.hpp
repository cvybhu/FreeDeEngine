#pragma once

#include "glIncludes.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

class ResourceManager;
namespace Globals {extern ResourceManager resources;}

#include "Texture.hpp"


class Mesh
{
public:
    Mesh();

    void loadToRAM(const std::string& filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();


    Texture* myTexture;

    const GLuint& getVAO() const;

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
