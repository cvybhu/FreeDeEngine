#pragma once

#include <glad/glad.h>
#include <Storage.hpp>
#include <Sprite3D.hpp>
#include <StaticMemPool.hpp>
#include <vector>
#include <Logger.hpp>


template <int maxSprite3DNum>
class Renderer
{
public:
    Renderer();

    void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

    Sprite3D& addSprite3D(int mesIndex);




private:
    StaticMemPool<Sprite3D, maxSprite3DNum>& spritesMem;

    std::vector<Sprite3D*> * meshes;


private: //static
    static bool loadedShaders;
    static void loadShaders();
};









//#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#


template <int maxSprite3DNum>
bool Renderer<maxSprite3DNum>::loadedShaders = false;


template <int maxSprite3DNum>
void Renderer<maxSprite3DNum>::loadShaders()
{
    Shader& texShader = Storage::shaders[shaderNames::tex];
    texShader.load(Storage::shaders.getFilePath(shaderNames::tex));


}


template <int maxSprite3DNum>
Renderer<maxSprite3DNum>::Renderer() : spritesMem(*(new StaticMemPool<Sprite3D, maxSprite3DNum>()))
{
    if(!loadedShaders)
        loadShaders();

    meshes = new std::vector<Sprite3D*>[MESH_MAX_COUNT+5];
}


template <int maxSprite3DNum>
Sprite3D& Renderer<maxSprite3DNum>::addSprite3D(int meshIndex)
{
    Sprite3D& sprite = *(new (spritesMem.getMem()) Sprite3D(meshIndex));

    sprite.myVecIndex = meshes[meshIndex].size();

    meshes[meshIndex].emplace_back(&sprite);

    return sprite;
}

template <int maxSprite3DNum>
void Renderer<maxSprite3DNum>::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader& tex = Storage::shaders[shaderNames::tex];
    tex.use();


    glActiveTexture(GL_TEXTURE0);

    tex.setMat4("view", viewMatrix);
    tex.setMat4("projection", projectionMatrix);



    for(int meshIndex = 0; meshIndex <= MESH_MAX_COUNT; meshIndex++)
    {
        std::vector<Sprite3D*>& sprites = meshes[meshIndex];

        if(sprites.empty())
            continue;



        Mesh& mesh = Storage::meshes[meshIndex];

        glBindTexture(GL_TEXTURE_2D, mesh.myTexture->getGLindx());
        glBindVertexArray(mesh.getVAO());

        for(Sprite3D* sprite : sprites)
        {
            tex.setMat4("model", sprite->model);
            glDrawArrays(GL_TRIANGLES, 0, mesh.getVertsNum());
        }
    }
}

