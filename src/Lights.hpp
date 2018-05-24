#pragma once

#include <glm/vec3.hpp>

struct PointLight
{
    glm::vec3 pos;

    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;
};



struct DirLight
{
    glm::vec3 color;

    glm::vec3 dir;
};

