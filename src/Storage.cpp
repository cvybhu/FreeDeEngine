#include <Storage.hpp>


#define initNames(namespaceName) namespace namespaceName {std::vector<std::string> filePaths; char initNames = (setup(), 0);}

initNames(shaderNames)
initNames(texNames)
initNames(meshNames)

#undef initNames



namespace Storage
{
    NamedResourceStorage<Texture, TEXTURE_MAX_COUNT> textures(texNames::filePaths);
    NamedResourceStorage<Mesh, MESH_MAX_COUNT> meshes(meshNames::filePaths);
    NamedResourceStorage<Shader, SHADER_MAX_COUNT> shaders(shaderNames::filePaths);
}
