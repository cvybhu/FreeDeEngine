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
    bool isPressed(int keyCode);
    bool isClicked(int keyCode);    //click is only once per press - next click happens after release

    void update();


    //callbacks
    void addKeyCallback(            void (*funcPtr)(GLFWwindow* win, int key, int scancode, int action, int mods));
    void addCharCallback(           void (*funcPtr)(GLFWwindow* win, unsigned int codepoint));
    void addCharModsCallback(       void (*funcPtr)(GLFWwindow* win, unsigned int codepoint, int mods));
    void addCursorPosCallback(      void (*funcPtr)(GLFWwindow* win, double xpos, double ypos));
    void addCursorEnterCallback(    void (*funcPtr)(GLFWwindow* win, int entered));
    void addMouseButtonCallback(    void (*funcPtr)(GLFWwindow* win, int button, int action, int mods));
    void addScrollCallback(         void (*funcPtr)(GLFWwindow* win, double xoffset, double yoffset));
    void addJoystickCallback(       void (*funcPtr)(int joy, int event));
    void addDropCallback(           void (*funcPtr)(GLFWwindow* window, int count, const char** paths));
    void addFramebufferSizeCallback(void (*funcPtr)(GLFWwindow* win, int newWidth, int newHeight));
}