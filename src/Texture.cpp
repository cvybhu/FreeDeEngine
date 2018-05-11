#include <Texture.hpp>

#include <Logger.hpp>


Texture::Texture() : width(0), height(0), nrChannels(0), isOnRAM(false), isOnGPU(false)
{




}



void Texture::loadToRAM(const char* filePath)
{
    data = stbi_load(filePath, &width, &height, &nrChannels, 0);

    if (!data)
    {
        Log << "[ERROR TEX] Failed to load texture " << filePath << "\n";
    }

    isOnRAM = true;

    Log << "[TEXLOAD] Succesfully loaded " << filePath << "!\n";
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

    isOnGPU = true;
}


void Texture::unloadFromRAM()
{
    stbi_image_free(data);

    isOnRAM = false;
}

void Texture::unloadFromGPU()
{
    //TODO
    isOnGPU = false;
}



const GLuint& Texture::getGLindx() const {return glIndx;}

const int& Texture::getWidth() const {return width;}
const int& Texture::getHeight() const {return height;}
const int& Texture::getNrChannels() const {return nrChannels;}

bool Texture::isLoadedToRAM()const{return isOnRAM;}
bool Texture::isLoadedToGPU()const{return isOnGPU;}


unsigned char* Texture::getPixel(const int& x, const int& y){return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}
const unsigned char* Texture::getPixel(const int& x, const int& y) const {return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}

