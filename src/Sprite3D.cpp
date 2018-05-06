#include "Sprite3D.hpp"

#include <glm/gtc/matrix_transform.hpp>

Sprite3D::Sprite3D(Mesh& theMesh) : myMesh(&theMesh)
{



}


glm::mat4 Sprite3D::getModelMatrix()
{
    glm::mat4 model;
    model = glm::translate(model, pos);
    return model;
}
