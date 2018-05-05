#include "glIncludes.hpp"
#include <cmath>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Globals.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

#include "debugFuncs.hpp"
#include "Mesh.hpp"


#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


bool init(GLFWwindow*& window)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif


    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);


    return true;
}



int main()
{
    GLFWwindow* window;
    if(!init(window))
    {
        cout << "No init for ya!" << endl;
        return -1;
    }


    Shader shader("src/shaders/tex.vs", "src/shaders/tex.fs");


    Mesh cube;
    cube.loadToRAM("mesh/cube.obj");
    cube.loadToGPU();

    Texture& container = Globals::resources.textures[texNames::container];
    container.loadToRAM("tex/container.jpg");
    container.loadToGPU();

    LN(cube.getVAO())
    LN(cube.verts.size())

    LN(sizeof(Mesh::Vertex)/8)
    LN(sizeof(float))

    Camera cam;
    cam.pos = glm::vec3(-6, 0, 0);
    cam.rot = glm::vec3(0, 0, 0);


    glm::mat4 model;
    glm::mat4 projection;
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.3f, 0.7f));
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // draw our first triangle


        // render container
        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, container.getGLindx());



        shader.setMat4("model", model);
        shader.setMat4("view", cam.getViewMatrix());
        shader.setMat4("projection", projection);

        glBindVertexArray(cube.getVAO());
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /*
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    */

    glfwTerminate();
    return 0;
}



void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

