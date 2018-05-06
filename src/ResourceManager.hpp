#pragma once

#include "Mesh.hpp"


#include "ResourceNames.hpp"
#include "NamedResourceStorage.hpp"

namespace texNames{extern std::vector<std::string> filePaths;}



class ResourceManager
{
public:
    ResourceManager() : textures(texNames::filePaths), meshes(meshNames::filePaths) {}

    NamedResourceStorage<Texture, 100> textures;
    NamedResourceStorage<Mesh, 100> meshes;


    void loadTextureToRAM(int index);
    void loadTextureToRAM(const std::string& filePath);

    //Model models[modelNames::modelsNumber];
};
