#pragma once

#include <Mesh.hpp>
#include <glm/mat4x4.hpp>

struct Sprite3D
{
    Sprite3D(Mesh* mineMesh) : myMesh(mineMesh) {}

    Mesh* const myMesh;
    glm::mat4 model;

    Sprite3D(const Sprite3D& b) = delete;   //no passing by value - its a pointer!!
    void operator=(const Mesh& b) = delete;
};
