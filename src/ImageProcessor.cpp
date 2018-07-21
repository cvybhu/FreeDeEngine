#include <ImageProcessor.hpp>
#include <Logger.hpp>

ImageProcessor::ImageProcessor(int width, int height, GLenum internalFormat, GLenum format, GLenum dataType, void* pixelData) 
{
    Width = width;
    Height = height;

    //Framebuffer creation
    glGenFramebuffers(1, &frameBuff);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuff);

    glGenTextures(1, &renderTexture);
    glBindTexture(GL_TEXTURE_2D, renderTexture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, pixelData);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

    //screen Quad creation
    float screenQuadVerts[] = {-1,1, -1,-1, 1,-1, -1,1, 1,-1,  1,1};

    glGenVertexArrays(1, &screenQuad.VAO);
    glBindVertexArray(screenQuad.VAO);

    glGenBuffers(1, &screenQuad.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuad.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVerts), &screenQuadVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
}

void ImageProcessor::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuff);
    glDisable(GL_DEPTH_TEST); 
    glBindVertexArray(screenQuad.VAO);
    glViewport(0, 0, Width, Height);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glFlush();
    glFinish();
}

ImageProcessor::~ImageProcessor()
{
    glDeleteFramebuffers(1, &frameBuff);
    glDeleteTextures(1, &renderTexture);
    glDeleteBuffers(1, &screenQuad.VBO);
    glDeleteVertexArrays(1, &screenQuad.VAO);
}