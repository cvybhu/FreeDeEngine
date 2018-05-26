#pragma once

#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <Shader.hpp>

class Renderer;

struct PointLight
{
public:
//Lighting data
    glm::vec3 pos;

    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;

//Shadow
    void setupShadow(int resolution, float farPlane = 100.f);
    void destroyShadow();

    void activateShadow();
    void deactivateShadow();


private:
    friend Renderer;
    struct
    {
        bool active = false;

        friend PointLight;
        friend Renderer;

        int resolution;

        GLuint fbuff;
        GLuint cubeMap;

        float farPlane;
    } shadow;

    void setupShadowRendering(Shader& shader);
};


struct DirLight
{
    glm::vec3 color;

    glm::vec3 dir;


    void setupShadow(glm::vec2 viewSize, glm::vec3 center, float farPlane = 100.f, glm::ivec2 resolution = {1024, 1024});
    void destroyShadow();

    void activateShadow();
    void deactivateShadow();

private:
    friend Renderer;

    struct 
    {
        bool active = false;

        glm::ivec2 resolution;

        GLuint fbuff;
        GLuint depth;

        float farPlane;
        glm::vec2 viewSize;
        glm::vec3 center;
    } shadow;

    void setupShadowRendering(Shader& shader);
    glm::mat4 getLightSpaceMat();
};

