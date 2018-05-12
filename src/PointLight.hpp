#pragma once

#include <glm/vec3.hpp>

class Renderer;


class PointLight
{
public:
    glm::vec3 pos;

    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;

private:
    friend Renderer;
    int myVecIndex;
};
