#include "ResourceManager.hpp"




void ResourceManager::loadTextureToRAM(int index)
{
    textures[index].loadToRAM(textures.getFilePath(index));
}


void ResourceManager::loadTextureToRAM(const std::string& filePath)
{
    loadTextureToRAM(textures.getIndex(filePath));
}

