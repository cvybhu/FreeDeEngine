#pragma once

#include "Sprite3D.hpp"

#include <vector>

class Renderer
{
public:
    Renderer();


    int addSprite3D(const Sprite3D& sprite);
    void removeSprite3D(int index);

    Sprite3D& getSprite3D(int index);

    void draw();

private:
    std::vector<Sprite3D> sprites;
    std::vector<int> freePlaces;
};


