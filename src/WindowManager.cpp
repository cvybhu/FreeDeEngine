#include "WindowManager.hpp"

#include <iostream>

GLFWwindow* WindowManager::window;
glm::vec2 WindowManager::mousePos, WindowManager::lastMousePos;
int WindowManager::width, WindowManager::height;



WindowManager::WindowManager()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
    #endif

    width = 1600;
    height = 900;


    window = glfwCreateWindow(width, height, "SuperEngine 3000", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);


    glfwSetCursorPos(window, lastMousePos.x, lastMousePos.y);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

WindowManager::operator GLFWwindow*(){return window;}



float WindowManager::getMouseX(){return mousePos.x;}
float WindowManager::getMouseY(){return mousePos.y;}
glm::vec2 WindowManager::getMousePos(){return mousePos;}

float WindowManager::getMouseDeltaX(){return mousePos.x - lastMousePos.x;}
float WindowManager::getMouseDeltaY(){return lastMousePos.y - mousePos.y;}
glm::vec2 WindowManager::getMouseDelta(){return glm::vec2(getMouseDeltaX(), getMouseDeltaY());}



bool WindowManager::isPressed(int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS);
}


int WindowManager::getWidth()
{
    return width;
}

int WindowManager::getHeight()
{
    return height;
}





void WindowManager::update()
{
    lastMousePos = mousePos;
}



//Callbacks

void WindowManager::framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    glViewport(0, 0, newWidth, newHeight);
}

void WindowManager::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    mousePos.x = xpos;
    mousePos.y = ypos;
}






