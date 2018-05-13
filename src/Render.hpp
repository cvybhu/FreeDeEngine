#pragma once

#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <stb_image.h>
#include <string>
#include <vector>

namespace Window
{
    extern GLFWwindow* window;
    extern int height, width;

    extern glm::vec2 mousePos;
    extern glm::vec2 mouseDelta;

    void init();
    inline bool isPressed(int keyCode){return (glfwGetKey(window, keyCode) == GLFW_PRESS);}

    void update();
}



struct Texture
{
    unsigned char* data;
    int width, height, nrChannels;
    GLuint glIndx;
    bool isOnRAM, isOnGPU;

    void loadToRAM(const char* filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();

    unsigned char* getPixel(const int& x, const int& y);              //x goes L->R y goes U->D thats kinda prototype
    const unsigned char* getPixel(const int& x, const int& y) const;  //const-safe version xd
};

namespace Storage
{
    Texture& getTex(const char* name);
}





struct Shader
{
    GLuint program;

    void load(const std::string& path);

    void use(){glUseProgram(program);}

    template <class... Args> void set1Int(const char* name, Args... args)  {glUniform1i(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set2Int(const char* name, Args... args)  {glUniform2i(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set3Int(const char* name, Args... args)  {glUniform3i(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set4Int(const char* name, Args... args)  {glUniform4i(glGetUniformLocation(program, name), args...);}

    template <class... Args> void set1Float(const char* name, Args... args)  {glUniform1f(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set2Float(const char* name, Args... args)  {glUniform2f(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set3Float(const char* name, Args... args)  {glUniform3f(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set4Float(const char* name, Args... args)  {glUniform4f(glGetUniformLocation(program, name), args...);}

    void setVec2(const char* name, glm::vec2 vec) {glUniform2f(glGetUniformLocation(program, name), vec.x, vec.y);}
    void setVec3(const char* name, glm::vec3 vec) {glUniform3f(glGetUniformLocation(program, name), vec.x, vec.y, vec.z);}
    void setVec4(const char* name, glm::vec4 vec) {glUniform4f(glGetUniformLocation(program, name), vec.x, vec.y, vec.z, vec.w);}

    void setMat4(const char* name, const glm::mat4& theMat) { glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(theMat));}
};

namespace Storage
{
    Shader& getShader(const char* filePath);
}




struct Mesh
{
    void loadToRAM(const char* filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();


    Texture* diffTexture;
    Texture* specTexture;

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    std::vector<Vertex> verts;
    int vertsNum;
    GLuint VBO, VAO;
};

namespace Storage
{
    Mesh& getMesh(const char* filePath);
}




struct FreeCam
{
    FreeCam(glm::vec3 position = {0, 0, 0}, glm::vec2 rotation = {0, 0}, float speed = 10);

    glm::vec3 pos;
    glm::vec2 rot;  //yaw pitch

    float fov;

    float sensitivity;
    float speed;


    void handleCameraRot(glm::vec2 mouseDelta);
    void handleMovement(float deltaTime);

    void moveForward(float deltaTime);
    void moveBack(float deltaTime);
    void moveRight(float deltaTime);
    void moveLeft(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);


//openGL stuff
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
};




struct PointLight
{
    glm::vec3 pos;

    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;
};



struct DirLight
{
    glm::vec3 color;

    glm::vec3 dir;
};


