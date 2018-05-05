#include "Mesh.hpp"

using namespace std;

#include <fstream>
#include <vector>

Mesh::Mesh()
{

}


void Mesh::loadToRAM(const std::string& filePath)
{
    ifstream file(filePath);

    string input;

    vector<glm::vec3> vecPoses;
    vector<glm::vec3> vecNormals;
    vector<glm::vec2> texCoords;

    while(file >> input)
    {
        if(input != "v" && input != "vt" && input != "vn" && input != "f")
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
                continue;
            }
        }
    }

    file.close();
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

    glBindVertexArray(0);
}

void Mesh::unloadFromGPU()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}




const GLuint& Mesh::getVAO() const {return VAO;}
