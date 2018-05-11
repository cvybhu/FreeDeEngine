#pragma once

#include <glad/glad.h>
#include <stb_image.h>


class Texture
{
public:
    Texture();

    void loadToRAM(const char* filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();

    const GLuint& getGLindx() const;

    const int& getWidth() const;
    const int& getHeight() const;
    const int& getNrChannels() const;

    bool isLoadedToRAM() const;
    bool isLoadedToGPU() const;

    unsigned char* getPixel(const int& x, const int& y);              //x goes L->R y goes U->D thats kinda prototype
    const unsigned char* getPixel(const int& x, const int& y) const;  //const-safe version xd

private:
    unsigned char* data;
    int width, height, nrChannels;
    GLuint glIndx;

    bool isOnRAM, isOnGPU;
};



