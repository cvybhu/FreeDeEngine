#pragma once

#include <Sprite3D.hpp>
#include <PointLight.hpp>
#include <DynMemPool.hpp>
#include <vector>
#include <Shader.hpp>


class Renderer
{
public:
    Renderer(int maxSprite3DNum, int maxPointLightsNum);

    void draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

    Sprite3D& addSprite3D(int mesIndex);
    void removeSprite3D(Sprite3D& sprite);

    PointLight& addPointLight();
    void removePointLight(PointLight& light);


    glm::vec3 ambientLight;


private:
    DynMemPool<Sprite3D> spritesMem;
    DynMemPool<PointLight> pointLightsMem;

    std::vector<Sprite3D*> * meshes;
    std::vector<PointLight*> pointLights;

    template <class T, class... Args>
    T& addThing(DynMemPool<T>& memPool, std::vector<T*>& vec, Args... args);

    template <class T>
    void removeThing(T& thing, DynMemPool<T>& memPool, std::vector<T*>& vec);

    void loadLights(Shader& shader);

    void drawLights(glm::mat4& projectionMatrix, glm::mat4& viewMatrix);


private: //static
    static bool loadedShaders;
    static void loadShaders();
};
