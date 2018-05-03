#pragma once

#include "glIncludes.hpp"
#include <stb_image.h>
#include <string>
#include <iostream>



class Texture
{
public:
    Texture();

    void loadToRAM(const std::string& filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();

    GLuint getGLindx();

    const int& width;
    const int& height;

private:
    unsigned char* data;
    int textureWidth, textureHeight, nrChannels;
    GLuint glIndx;
};



