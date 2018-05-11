#include <iostream>
#include <Window.hpp>
#include <Shader.hpp>
#include <Storage.hpp>
#include <FreeCam.hpp>
#include <Renderer.hpp>
#include <TimeTeller.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace std;



int main()
{
    Window::init();


    Mesh& planeMesh = Storage::meshes[meshNames::plane];
    planeMesh.loadToRAM("mesh/plane.obj");
    planeMesh.loadToGPU();

    Renderer<10> render;

    Sprite3D& plane = render.addSprite3D(meshNames::plane);


    FreeCam cam({0, 10, 0});


    double lastFrameTime = glfwGetTime();
    double deltaTime = 0;

    TimeTeller frameTimeTeller("frame time", 0.5);
    TimeTeller renderTimeTeller("render time", 0.5);
    while (!glfwWindowShouldClose(Window::window))
    {
        //time
        deltaTime = glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();

        frameTimeTeller.startMeasuring();


        //input
        if(Window::isPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(Window::window, true);

        cam.handleMovement(deltaTime);


        // render
        renderTimeTeller.startMeasuring();
        render.draw(cam.getProjectionMatrix(), cam.getViewMatrix());
        renderTimeTeller.stopMeasuring();


        //time
        frameTimeTeller.stopMeasuring();
        frameTimeTeller.tell();
        renderTimeTeller.tell();

        //window events
        Window::update();

        glfwSwapBuffers(Window::window);
    }


    return 0;
}

