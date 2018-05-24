#pragma once

#include <Shader.hpp>
#include <Texture.hpp>
#include <Mesh.hpp>


namespace Storage
{
    Shader& getShader(const char* filePath);

    Texture& getTex(const char* filePath);
    CubeTexture& getCubeTex(const char* fileName);
    
    Mesh& getMesh(const char* filePath);
}
