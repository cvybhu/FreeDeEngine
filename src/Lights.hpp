#pragma once

#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <Shader.hpp>


struct PointLight
{
    glm::vec3 pos;

    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;

    struct Shadow
    {
    public:
        void create(int resolution, float FarPlane = 100.f);
        void destroy();
        void loadMats2Shader(Shader& shader);

        bool active = false;

        float farPlane;
    private:
        int res;
        GLuint fbuff;
        GLuint cubeMap;
    };

    Shadow shadow;
};


struct DirLight
{
    glm::vec3 color;

    glm::vec3 dir;
};

