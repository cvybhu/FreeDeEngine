#pragma once

#include <GLFW/glfw3.h>

void ImGui_impl_init(GLFWwindow* window);
void ImGui_impl_deinit();
void ImGui_impl_startFrame();
void ImGui_impl_endFrame(int windowWidth, int windowHeight);