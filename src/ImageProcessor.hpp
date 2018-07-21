#pragma once

#include <glad/glad.h>
#include <Shader.hpp>

class ImageProcessor    //Processes image (wrapper for setting up rendering to texture and rendering screen quad over it)
{
public:
    ImageProcessor(int width, int height, GLenum internalFormat, GLenum format, GLenum dataType, void* pixelData = 0);
    
    void render();

    GLuint renderTexture;

    ~ImageProcessor();

private:
    GLuint frameBuff;
    int Width, Height;

    struct{
        GLuint VBO, VAO;
    } screenQuad;
};