#include <Renderer.hpp>

#include <glad/glad.h>
#include <Storage.hpp>
#include <Logger.hpp>



void Renderer::loadShaders()
{
    auto indexes = {shaderNames::tex, shaderNames::light};

    for(int i : indexes)
    {
        Shader& s = Storage::shaders[i];
        s.load(Storage::shaders.getFilePath(i));
    }
}

bool Renderer::loadedShaders = false;


Renderer::Renderer(int maxSprites3DNum, int maxPointLightsNum) : spritesMem(maxSprites3DNum), pointLightsMem(maxPointLightsNum)
{
    if(!loadedShaders)
        loadShaders();

    meshes = new std::vector<Sprite3D*>[MESH_MAX_COUNT+5];

    ambientLight = glm::vec3(0.3);
}


template <class T, class... Args>
T& Renderer::addThing(DynMemPool<T>& memPool, std::vector<T*>& vec, Args... args)
{
    T& thing = *(new (memPool.getMem()) T(args...));

    thing.myVecIndex = vec.size();
    vec.emplace_back(&thing);

    return thing;
}

template <class T>
void Renderer::removeThing(T& thing, DynMemPool<T>& memPool, std::vector<T*>& vec)
{
    if(thing.myVecIndex == (int)vec.size() - 1)
    {
        vec.pop_back();
    }
    else
    {
        vec.back()->myVecIndex = thing.myVecIndex;
        std::swap(vec[thing.myVecIndex], vec.back());
    }

    memPool.freeMem(&thing);
}




Sprite3D& Renderer::addSprite3D(int meshIndex)
{
    return addThing(spritesMem, meshes[meshIndex], meshIndex);
}


void Renderer::removeSprite3D(Sprite3D& sprite)
{
    removeThing(sprite, spritesMem, meshes[sprite.meshIndex]);
}


PointLight& Renderer::addPointLight()
{
    return addThing(pointLightsMem, pointLights);
}

void Renderer::removePointLight(PointLight& light)
{
    removeThing(light, pointLightsMem, pointLights);
}
