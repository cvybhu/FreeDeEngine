#pragma once

#include "Texture.hpp"
#include "Model.hpp"


#include "ResourceNames.hpp"
#include "NamedResourceStorage.hpp"

namespace texNames{extern std::vector<std::string> filePaths;}



class ResourceManager
{
public:
    ResourceManager() : textures(texNames::filePaths) {}

    NamedResourceStorage<Texture, 100> textures;


    void loadTextureToRAM(int index);
    void loadTextureToRAM(const std::string& filePath);

    //Model models[modelNames::modelsNumber];
};
