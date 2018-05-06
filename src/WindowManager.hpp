#pragma once

#include "glIncludes.hpp"
#include <glm/vec2.hpp>

class WindowManager
{
public:
    WindowManager();

    operator GLFWwindow*();

    void update();

    glm::vec2 getMousePos();
    float getMouseX();
    float getMouseY();

    glm::vec2 getMouseDelta();
    float getMouseDeltaX();
    float getMouseDeltaY();

    bool isPressed(int key);

    int getWidth();
    int getHeight();

private:
    static GLFWwindow* window;
    static glm::vec2 mousePos;
    static glm::vec2 lastMousePos;
    static int width;
    static int height;

public:
    static void framebuffer_size_callback(GLFWwindow* window, int newWidth, int newHeight);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
};
