#include "glIncludes.hpp"
#include <cmath>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Globals.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "utils.hpp"

#include "debugFuncs.hpp"


#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


int main()
{
    Shader shader("src/shaders/tex.vs", "src/shaders/tex.fs");


    Mesh& plane = Globals::resources.meshes[meshNames::plane];
    plane.loadToRAM("mesh/plane.obj");
    plane.loadToGPU();

    Sprite3D& sprite = Globals::render.addSprite3D(meshNames::plane);

    sprite.myMesh->loadToGPU();



    Camera cam;
    cam.pos = glm::vec3(0, 10, 0);
    cam.rot = glm::vec3(0, 0, 0);
    cam.speed = 10.0;



    float yaw = 0, pitch = 0, roll = 0;

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)Globals::window.getWidth() / (float)Globals::window.getHeight(), 0.1f, 100.0f);


    float lastFrameTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(Globals::window))
    {
        float deltaTime = (float)glfwGetTime() - lastFrameTime; lastFrameTime = glfwGetTime();
        // /\ delta time since last frame


        // input \/
        if(Globals::window.isPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(Globals::window, true);

        cam.handleMovement(deltaTime);



        float speed = 1.0;

        if(Globals::window.isPressed(GLFW_KEY_1))yaw += speed*deltaTime;
        if(Globals::window.isPressed(GLFW_KEY_2))yaw -= speed*deltaTime;
        if(Globals::window.isPressed(GLFW_KEY_3))pitch += speed*deltaTime;
        if(Globals::window.isPressed(GLFW_KEY_4))pitch -= speed*deltaTime;
        if(Globals::window.isPressed(GLFW_KEY_5))roll += speed*deltaTime;
        if(Globals::window.isPressed(GLFW_KEY_6))roll -= speed*deltaTime;


        Globals::window.update();
        //input ^



        // render \/
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plane.myTexture->getGLindx());


        shader.setMat4("model", utils::getYawPitchRollMat(yaw, pitch, roll));
        shader.setMat4("view", cam.getViewMatrix());
        shader.setMat4("projection", projection);

        glBindVertexArray(plane.getVAO());
        glDrawArrays(GL_TRIANGLES, 0, plane.getVertsNum());

        // -------------------------------------------------------------------------------
        glfwSwapBuffers(Globals::window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}




