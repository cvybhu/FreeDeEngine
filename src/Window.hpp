#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

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