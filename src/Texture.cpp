#include "Texture.hpp"


Texture::Texture() : width(textureWidth), height(textureHeight)
{




}



void Texture::loadToRAM(const std::string& filePath)
{
    data = stbi_load(filePath.c_str(), &textureWidth, &textureHeight, &nrChannels, 0);

    if (!data)
    {
        std::cout << "Failed to load texture " << filePath << std::endl;
    }
}



void Texture::loadToGPU()
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_2D, glIndx);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}


void Texture::unloadFromRAM()
{
    stbi_image_free(data);
}

void Texture::unloadFromGPU()
{
    //TODO
}



GLuint Texture::getGLindx()
{
    return glIndx;

}

