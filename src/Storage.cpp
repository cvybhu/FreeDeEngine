#include <Storage.hpp>

#include <string>
#include <map>

//Storage stuff
namespace Storage
{
    std::map<std::string, Shader> shaders;
    std::map<std::string, Texture> textures;
    std::map<std::string, Mesh> meshes;
    std::map<std::string, CubeTexture> cubeTextures;


    Shader& getShader(const char* filePath)
    {
        return shaders[filePath];
    }


    Texture& getTex(const char* filePath)
    {
        return textures[filePath];
    }


    Mesh& getMesh(const char* filePath)
    {
        return meshes[filePath];
    }

    CubeTexture& getCubeTex(const char* fileName)
    {
        return cubeTextures[fileName];
    }
}

