#pragma once

#include <glm/mat4x4.hpp>

class Renderer;



class Sprite3D
{
public:
    Sprite3D(int MeshIndex) : meshIndex(MeshIndex), model(1){}

    const int meshIndex;

    glm::mat4 model;



private:
    friend Renderer;
    int myVecIndex;
};
