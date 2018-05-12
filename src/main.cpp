#include <iostream>
#include <Window.hpp>
#include <Shader.hpp>
#include <Storage.hpp>
#include <FreeCam.hpp>
#include <Renderer.hpp>
#include <TimeTeller.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace std;


#include <DynMemPool.hpp>


int main()
{
    Window::init();


    auto meshes2Load = {meshNames::plane, meshNames::light};

    for(int i : meshes2Load)
    {
        Mesh& mesh = Storage::meshes[i];
        mesh.loadToRAM(Storage::meshes.getFilePath(i));
        mesh.loadToGPU();
    }



    Renderer render(2, 10);

    Sprite3D* plane = &render.addSprite3D(meshNames::plane);

    Sprite3D& lightSprite = render.addSprite3D(meshNames::light);
    lightSprite.model = glm::translate(lightSprite.model, glm::vec3(5, 5, 5));

    PointLight& light = render.addPointLight();
    light.pos = {5, 5, 5};
    light.color = glm::vec3(2.0);
    light.constant = 1.f;
    light.linear = 0.09f;
    light.quadratic = 0.032f;



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

