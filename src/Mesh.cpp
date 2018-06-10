#include <Mesh.hpp>

#include <glad/glad.h>
#include <Storage.hpp>
#include <fstream>
#include <Logger.hpp>


void Mesh::calculateTangents()
{
    for(int i = 0; i < (int)verts.size(); i += 3)
    {
        Vertex* curVerts = &verts[i];

        glm::vec3 edge1 = curVerts[1].pos - curVerts[0].pos;
        glm::vec3 edge2 = curVerts[2].pos - curVerts[0].pos;

        glm::vec2 deltaUV1 = curVerts[1].texCoords - curVerts[0].texCoords;
        glm::vec2 deltaUV2 = curVerts[2].texCoords - curVerts[0].texCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent =  (edge1 * deltaUV2.y   - edge2 * deltaUV1.y)*f;
        tangent = glm::normalize(tangent);

        glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        curVerts[0].tangent = curVerts[1].tangent = curVerts[2].tangent = tangent;
        curVerts[0].bitangent = curVerts[1].bitangent = curVerts[2].bitangent = bitangent;
    }
}

void Mesh::loadToRAM(const char* filePath)
{
    std::ifstream file(filePath);

    std::string input;

    std::vector<glm::vec3> vecPoses;
    std::vector<glm::vec3> vecNormals;
    std::vector<glm::vec2> texCoords;

    auto loadTex = [&file](Texture*& tex){
        std::string texPath;
        file >> texPath;
        tex = &Storage::getTex(texPath.c_str());

        if(!tex->isOnRAM)
            tex->loadToRAM(texPath.c_str());
    };


    while(file >> input)
    {

        if(input != "v" && input != "vt" && input != "vn" && input != "f" && input != "albedoTex" && input != "metalTex" && input != "roughTex" && input != "normTex" && input != "ambientOccTex")
            continue;

        if(input == "v")
        {
            glm::vec3 pos;
            file >> pos.x >> pos.y >> pos.z;
            vecPoses.emplace_back(pos);
            continue;
        }

        if(input == "vn")
        {
            glm::vec3 normal;
            file >> normal.x >> normal.y >> normal.z;
            vecNormals.emplace_back(normal);
            continue;
        }


        if(input == "vt")
        {
            glm::vec2 coord;
            file >> coord.x >> coord.y;
            texCoords.emplace_back(coord);
            continue;
        }

        if(input == "f")
        {
            for(int i = 0; i < 3;i++)
            {
                Vertex v;
                int index; char spacer;
                file >> index; file >> spacer;
                v.pos = vecPoses[index-1];
                file >> index; file >> spacer;
                v.texCoords = texCoords[index-1];
                file >> index;
                v.normal = vecNormals[index-1];

                verts.emplace_back(v);
            }

            continue;
        }

        if(input == "albedoTex")
        {
            loadTex(albedoTex);
            continue;
        }

        if(input == "metalTex")
        {
            loadTex(metallicTex);
            continue;
        }

        if(input == "roughTex")
        {
            loadTex(roughnessTex);
            continue;
        }

        if(input == "normTex")
        {
            loadTex(normalTex);
            continue;
        }

        if(input == "ambientOccTex")
        {
            loadTex(ambientOccTex);
            continue;
        }
    }

    file.close();

    calculateTangents();

    std::cout << "[MESHLOAD]Succesfully loaded " << filePath << " (" << verts.size() << " vertices)\n";

    vertsNum = verts.size();
}

void Mesh::unloadFromRAM()
{
    verts.clear(); verts.shrink_to_fit();
}

void Mesh::loadToGPU()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),&indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    //tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    glBindVertexArray(0);


    auto checkNLoad = [](Texture* tex, bool fixGamma = false)
    {
        if(tex != nullptr && !tex->isOnGPU)
            tex->loadToGPU(fixGamma);
    };

    checkNLoad(albedoTex, true);
    checkNLoad(metallicTex);
    checkNLoad(roughnessTex);
    checkNLoad(normalTex);
    checkNLoad(ambientOccTex);
    checkNLoad(displacementTex);
    checkNLoad(emmisionTex);
}


void Mesh::unloadFromGPU()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

