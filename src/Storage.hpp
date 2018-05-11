#pragma once

#include <ResourceNames.hpp>
#include <NamedResourceStorage.hpp>

#include <Shader.hpp>
#include <Texture.hpp>
#include <Mesh.hpp>


#define SHADER_MAX_COUNT 8
#define TEXTURE_MAX_COUNT 100
#define MESH_MAX_COUNT 100


namespace Storage
{
    extern NamedResourceStorage<Shader, SHADER_MAX_COUNT> shaders;
    extern NamedResourceStorage<Texture, TEXTURE_MAX_COUNT> textures;
    extern NamedResourceStorage<Mesh, TEXTURE_MAX_COUNT> meshes;

}
