#include "Texture.hpp"


Texture::Texture() : width(0), height(0), nrChannels(0)
{




}



void Texture::loadToRAM(const std::string& filePath)
{
    data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

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



const GLuint& Texture::getGLindx() const {return glIndx;}

const int& Texture::getWidth() const {return width;}
const int& Texture::getHeight() const {return height;}
const int& Texture::getNrChannels() const {return nrChannels;}


unsigned char* Texture::getPixel(const int& x, const int& y){return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}
const unsigned char* Texture::getPixel(const int& x, const int& y) const {return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}

