#include <Game.hpp>

#include <Window.hpp>
#include <Storage.hpp>

#include <Renderer.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <Camera.hpp>
#include <Logger.hpp>
#include <Utils.hpp>

#include <imgui/imgui.h>

namespace Game
{
    Renderer render;
    FreeCam cam;

    Sprite3D* plane;
    PointLight *light0, *light1;

    bool isImGuiOpened = false;

    void init()
    {
        cam.pos = {0, 4, 0};

        render.init({Window::width, Window::height}, 10);

        CubeTexture& skyboxMountLake = Storage<CubeTexture>::get("src/tex/mountainsCube");
        skyboxMountLake.loadToRAM("tex/mountainsCube");
        skyboxMountLake.loadToGPU(true);
        render.currentSkybox = &skyboxMountLake;

        Sprite3D* floor = render.addSprite3D(Storage<Mesh>::get("mesh/floor.obj"));
        floor->model = glm::translate(glm::mat4(1), glm::vec3(0, 0, -1));


        Sprite3D* pbrCube = render.addSprite3D(Storage<Mesh>::get("mesh/pbrCube.obj"));
        pbrCube->model = glm::mat4(1);

        light0 = render.addPointLight();
        light0->pos = {2, 2, 2};
        light0->color = {0.5, 0, 0}; light0->color *= 30;
        light0->setupShadow(512, 100);
        light0->activateShadow();

        
        light1 = render.addPointLight();
        light1->pos = {-2, -3, 4};
        light1->color = {0, 0.5, 0.5}; light1->color *= 30.f;
        light1->setupShadow(666, 100);
        light1->activateShadow();

        render.dirLight.color = glm::vec3(glm::vec2(0.5), 0.4) * 3.f;
        render.dirLight.dir = glm::vec3(0, -1, -1);

        //render.dirLight.setupShadow({50, 50}, {7, 0, 0}, 100.f, {1024, 1024});
        //render.dirLight.activateShadow();

        Sprite3D* cerber = render.addSprite3D(Storage<Mesh>::get("mesh/cerberus.obj"));
        cerber->model = glm::rotate(glm::translate(glm::scale(glm::mat4(1), glm::vec3(5)), glm::vec3(0, 0, 1)), glm::radians(90.f), glm::vec3(1, 0, 0));
        Sprite3D* cone = render.addSprite3D(Storage<Mesh>::get("mesh/cone.obj"));
        cone-> model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(4+1, -3, 0.9)), glm::vec3(0.25));
    }

    void update(float deltaTime)
    {
        light0->pos = glm::rotate(light0->pos, deltaTime, glm::vec3(0, 0, 1));
        light1->pos = glm::rotate(light1->pos, -deltaTime*0.5f, glm::vec3(0, 0, 1));

        if(Window::isClicked(GLFW_KEY_TAB))
            isImGuiOpened = !isImGuiOpened;

        if(isImGuiOpened)
            glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


        if(!isImGuiOpened)
            cam.handleMovement(deltaTime);
    }

    void draw()
    {
        render.draw(cam.getViewMatrix(), cam.getProjectionMatrix());

        if(isImGuiOpened)
        {
            ImGui::Begin("Control panel");

            ImGui::SliderFloat("exposure", &render.exposure, 0, 1);   
            ImGui::SliderFloat("Bloom min Brightness", &render.bloomMinBrightness, 0, 15);
            
            ImGui::End();
        }
    }
}

