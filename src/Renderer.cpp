#include "Renderer.hpp"

#include "WindowManager.hpp"
#include "ResourceManager.hpp"
namespace Globals
{
    extern WindowManager window;
    extern ResourceManager resources;
}

#include <glm/gtc/matrix_transform.hpp>


Renderer::Renderer() : texShader("src/shaders/tex.vs", "src/shaders/tex.fs")
{



}


Sprite3D& Renderer::addSprite3D(int meshIndex)
{
    Sprite3D* sprite = new (spriteMemPool.getMem())Sprite3D(Globals::resources.meshes[meshIndex]);
    return *sprite;
}



void Renderer::draw()
{



}

