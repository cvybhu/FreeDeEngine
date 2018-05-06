#pragma once

#include "Sprite3DBase.hpp"

#include "Mesh.hpp"

#include <glm/mat4x4.hpp>


class Sprite3D : public Sprite3DBase
{
public:
    Sprite3D(Mesh& theMesh);

    //glm::vec3 pos; x y z       -- this comes from base class
    //glm::vec3 rot; yaw pitch roll

    Mesh* const myMesh;

    //openGL Stuff
    glm::mat4 getModelMatrix();
};
