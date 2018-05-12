#pragma once

class Renderer;

class DirLight
{
public:
    glm::vec3 color;

    glm::vec3 dir;



private:
    friend Renderer;
    int myVecIndex;
};
