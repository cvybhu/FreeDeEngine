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

namespace Game
{
    Renderer render;
    FreeCam cam;

    Sprite3D* plane;
    PointLight *shadowedLight;
    PointLight* shadowedLight2;


    void init()
    {
        cam.pos = {0, 4, 0};

        render.init({Window::width, Window::height}, 10);
        render.bloomMinBrightness = 1.5;

        CubeTexture& skyboxMountLake = Storage::getCubeTex("src/tex/mountainsCube");
        skyboxMountLake.loadToRAM("tex/mountainsCube");
        skyboxMountLake.loadToGPU(true);
        render.currentSkybox = &skyboxMountLake;

    

        Sprite3D* pbrCube = render.addSprite3D(Storage::getMesh("mesh/pbrCube.obj"));
        pbrCube->model = glm::mat4(1);

        PointLight* light0 = render.addPointLight();
        light0->pos = {2, 2, 2};
        light0->color = {0.5, 0.5, 0}; light0->color *= 10;
        Sprite3D* light0sprite = render.addSprite3D(Storage::getMesh("mesh/light.obj"));
        light0sprite->model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(light0->pos)), glm::vec3(0.3));

        PointLight* light1 = render.addPointLight();
        light1->pos = {-3, -3, 4};
        light1->color = {0, 0.5, 0.5}; light1->color *= 10.f;
        Sprite3D* light1sprite = render.addSprite3D(Storage::getMesh("mesh/light.obj"));
        light1sprite->model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(light1->pos)), glm::vec3(0.3));

        render.dirLight.color = glm::vec3(glm::vec2(0.5), 0.4) * 3.f;
        render.dirLight.dir = glm::vec3(0, -1, -1);

        //render.dirLight.setupShadow({50, 50}, {7, 0, 0}, 100.f, {1024, 1024});
        //render.dirLight.activateShadow();
    }

    void exposureTesting(float deltaTime)
    {

        float exposureSensitivity = 0.5;
        if(Window::isPressed(GLFW_KEY_1))
        {
            render.exposure -= deltaTime * exposureSensitivity;
            if(render.exposure < 0)
                render.exposure = 0;
            std::cout << "Exposure: " << render.exposure << '\n';
        }

        if(Window::isPressed(GLFW_KEY_2))
        {
            render.exposure += deltaTime * exposureSensitivity;
            if(render.exposure > 1)
                render.exposure = 1;
            std::cout << "Exposure: " << render.exposure << '\n';
        }
    }

    void bloomBrightTesting(float deltaTime)
    {
        float sensitivity = 0.5;
        if(Window::isPressed(GLFW_KEY_3))
        {
            render.bloomMinBrightness -= deltaTime * sensitivity;
            if(render.bloomMinBrightness < 0)
                render.bloomMinBrightness = 0;
            std::cout << "bloomMinBrightness: " << render.bloomMinBrightness << '\n';
        }

        if(Window::isPressed(GLFW_KEY_4))
        {
            render.bloomMinBrightness += deltaTime * sensitivity;
            std::cout << "bloomMinBrightness: " << render.bloomMinBrightness << '\n';
        }
    }

    void update(float deltaTime)
    {
        cam.handleMovement(deltaTime);

        exposureTesting(deltaTime);
        bloomBrightTesting(deltaTime);

        auto centr = glm::vec3(20.5, -1, -1);

        auto curVec = shadowedLight->pos - centr;
        curVec = glm::rotate(curVec, deltaTime, glm::vec3(0, 0, 1));
        //shadowedLight->pos = centr + curVec;

        //shadowedLight2->pos += sin(glfwGetTime()) *0.01;


        if(Window::isPressed(GLFW_KEY_P))
            std::cout << "Camera pos: (" << cam.pos.x << ' ' << cam.pos.y << ' ' << cam.pos.z << "\n";
    }

    void draw()
    {
        render.draw(cam.getViewMatrix(), cam.getProjectionMatrix());
    }
}

