#pragma once


#include <vector>
#include <string>


class Model
{
public:
    Model();

    void loadToRAM(const std::string& filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();

private:
};
