#pragma once
#include <glad/glad.h>


struct Texture //holds pixel data. possible to load from file and on GPU
{
    unsigned char* data;    //data is stored as 1D array (R,G,B,A,X,  R,G,B,A,X, etc...)
    int width, height;
    int nrChannels;         //R G B is 3 channels, with alpha its 4 etc.

    GLuint glIndx;

    bool isOnRAM, isOnGPU;
    bool isNormalMap;

    void loadToRAM(const char* filePath);
    void loadToRamAsNormalMap(const char* filePath);
    void loadToGPU(bool fixGamma = false);

    void unloadFromRAM();
    void unloadFromGPU();

    unsigned char* getPixel(const int& x, const int& y);              //x goes L->R y goes D->U thats kinda prototype
    const unsigned char* getPixel(const int& x, const int& y) const;  //func gives pointer to place where pixel begins

private:
    void generateTexFile(const char* filePath); //generates .tex file from texture
};


struct CubeTexture
{
    unsigned char* data[6];
    int width, height;

    GLuint glIndx;

    bool isOnRAM, isOnGPU;

    void loadToRAM(const char* fileName);    //its expected that cubemap is like "fileName_up.jpg", _down, _front _back _right _left
    void loadToGPU(bool fixGamma = false);                       //also all of the files should have same resolution

    void unloadFromRAM();
    void unloadFromGPU();
};

struct EnvironmentTex //HDR skybox for IBL
{
    EnvironmentTex();

    float* data;
    int width, height;

    GLuint hdrTex;
    GLuint cubeMap;
    GLuint diffRadianceMap;
    GLuint prefilterMap;

    bool isOnRAM, isOnGPU;

    void loadToRAM(const char* filePath);
    void loadToGPU();
    void generateCubeMaps(int resolution);

    void unloadFromRAM();
    void unloadFromGPU();
};