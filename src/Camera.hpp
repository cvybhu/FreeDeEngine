#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
    glm::vec3 pos;
    glm::vec3 rot;

    float fov;

    glm::mat4 getViewMatrix();
};
