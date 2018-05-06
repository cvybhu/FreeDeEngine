#pragma once

#include "Sprite3D.hpp"
#include "Shader.hpp"
#include "StaticMemPool.hpp"

#include <vector>

#include <glm/mat4x4.hpp>

class Renderer
{
public:
    Renderer();

    Sprite3D& addSprite3D(int meshIndex);
    void removeSprite3D(int index);

    Sprite3D& getSprite3D(int index);

    void draw();

private:
    StaticMemPool<Sprite3D, 100> spriteMemPool;

    glm::mat4 projection;
    Shader texShader;
};


