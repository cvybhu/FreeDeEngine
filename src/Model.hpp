#pragma once

#include <string>


class Model
{
public:
    Model();

    void loadToRAM(const std::string& filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();


};
