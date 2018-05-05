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

    const GLuint& getGLindx() const;

    const int& getWidth() const;
    const int& getHeight() const;
    const int& getNrChannels() const;

    unsigned char* getPixel(const int& x, const int& y);              //x goes L->R y goes U->D thats kinda prototype
    const unsigned char* getPixel(const int& x, const int& y) const;  //const-safe version

private:
    unsigned char* data;
    int width, height, nrChannels;
    GLuint glIndx;
};



