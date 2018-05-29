#include <Texture.hpp>
#include <Logger.hpp>


void Texture::loadToRAM(const char* filePath)
{
    stbi_set_flip_vertically_on_load(true);
    //stbi_set_flip_horizontally_on_load(true);
    data = stbi_load(filePath, &width, &height, &nrChannels, 0);

    if (!data)
    {
        std::cout << "[ERROR TEX] Failed to load texture " << filePath << "\n";
        return;
    }

    isOnRAM = true;

    std::cout << "[TEXLOAD]Succesfully loaded " << filePath << "!\n";
}



void Texture::loadToGPU(bool fixGamma)
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_2D, glIndx);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum types[2];

    switch(nrChannels)
    {
    case 1:{
        types[0] = GL_R8;
        types[1] = GL_RED;
        break;
    }

    case 2:{
        types[0] = GL_RG8;
        types[1] = GL_RG;
        break;
    }

    case 3:{
        types[0] = fixGamma ? GL_SRGB8 : GL_RGB8;
        types[1] = GL_RGB;
        break;
    }

    case 4:{
        types[0] = fixGamma ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        types[1] = GL_RGBA;
        break;
    }
    };

    glTexImage2D(GL_TEXTURE_2D, 0, types[0], width, height, 0, types[1], GL_UNSIGNED_BYTE, data);
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

unsigned char* Texture::getPixel(const int& x, const int& y){return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}
const unsigned char* Texture::getPixel(const int& x, const int& y) const {return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}



void CubeTexture::loadToRAM(const char* fileName)
{
    int  nrChannels;
    stbi_set_flip_vertically_on_load(true);
    data[0] = stbi_load((std::string(fileName) + "_left.jpg").c_str(), &width, &height, &nrChannels, 0); //X+
    data[1] = stbi_load((std::string(fileName) + "_right.jpg").c_str(), &width, &height, &nrChannels, 0);  //X-
    data[2] = stbi_load((std::string(fileName) + "_back.jpg").c_str(), &width, &height, &nrChannels, 0);    //Y+
    data[3] = stbi_load((std::string(fileName) + "_front.jpg").c_str(), &width, &height, &nrChannels, 0);  //Y-
    data[4] = stbi_load((std::string(fileName) + "_up.jpg").c_str(), &width, &height, &nrChannels, 0);  //Z+
    data[5] = stbi_load((std::string(fileName) + "_down.jpg").c_str(), &width, &height, &nrChannels, 0); //Z-

    for(int i = 0; i < 6; i++)
        if(!data[i])
        {
            std::cout << "[TEXLOAD] Failed to load " << fileName << "!!!!!!!!!!!!!!\n";
        }

    isOnRAM = true;
}

void CubeTexture::loadToGPU(bool fixGamma)
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glIndx);

    for(int i = 0; i < 6; i++)
        if(fixGamma)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
        else
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    isOnGPU = true;
}


void CubeTexture::unloadFromRAM()
{
    for(int i = 0; i < 6; i++)
        stbi_image_free(data[i]);

    isOnRAM = false;
}

void CubeTexture::unloadFromGPU()
{
    //TODO

    isOnGPU = false;
}

