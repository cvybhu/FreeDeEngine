#include <iostream>
#include <Window.hpp>
#include <Shader.hpp>
#include <Storage.hpp>
#include <FreeCam.hpp>
#include <Renderer.hpp>
#include <TimeTeller.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std;




int main()
{
    Window::init();


    auto meshes2Load = {meshNames::plane};

    for(int i : meshes2Load)
    {
        Mesh& mesh = Storage::meshes[i];
        mesh.loadToRAM(Storage::meshes.getFilePath(i));
        mesh.loadToGPU();
    }



    Renderer render(10, 10);

    Sprite3D& plane = render.addSprite3D(meshNames::plane);

    PointLight& light = render.addPointLight();
    light.pos = {5, 5, 5};
    light.color = glm::vec3(1.0);
    light.constant = 1.f;
    light.linear = 0.05f;
    light.quadratic = 0.004f;

    PointLight& light2 = render.addPointLight();
    light2.pos = {3, 1, 4};
    light2.color = glm::vec3(0.0);
    light2.constant = 1.f;
    light2.linear = 0.07f;
    light2.quadratic = 0.004f;



    FreeCam cam({0, 10, 0});


    double lastFrameTime = glfwGetTime();
    double deltaTime = 0;

    TimeTeller frameTimeTeller("frame time", 1.5);
    TimeTeller renderTimeTeller("render time", 1.5);
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

        //game updates
        light.pos = glm::rotate(light.pos, (float)deltaTime, glm::vec3(0, 0, 1));
        light2.pos = glm::rotate(light2.pos, (float)deltaTime, glm::vec3(0.5, -0.5, 0.5));
        //plane.model = glm::rotate(plane.model, (float)deltaTime, glm::vec3(0, 0, 1));


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

