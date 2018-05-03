#pragma once

#include "Model.hpp"
#include <iostream>

class Sprite3D
{
public:
    Sprite3D();

    Model* model;
    float pos[3];
    float rotation[3];
};
