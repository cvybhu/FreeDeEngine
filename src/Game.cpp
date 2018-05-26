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


    void init()
    {
        render.init({Window::width, Window::height}, 10);
        render.bloomMinBrightness = 1.5;

        CubeTexture& skyboxMountLake = Storage::getCubeTex("src/tex/mountainsCube");
        skyboxMountLake.loadToRAM("tex/mountainsCube");
        skyboxMountLake.loadToGPU(true);
        render.currentSkybox = &skyboxMountLake;

        plane = render.addSprite3D(Storage::getMesh("mesh/spacePlane.obj"));
        plane->model = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(4, -4, 2)), (float)glfwGetTime(), glm::vec3(0, 0, 1));

        Sprite3D* stonePlace = render.addSprite3D(Storage::getMesh("mesh/stonePlace.obj"));
        stonePlace->model = glm::translate(glm::mat4(1), glm::vec3(0, -12, 0));

        Sprite3D* pointShadowTest = render.addSprite3D(Storage::getMesh("mesh/pointShadowTest.obj"));
        pointShadowTest->model = glm::translate(glm::mat4(1), glm::vec3(20, 0, 0));

        PointLight* light = render.addPointLight();
        light->pos = {5, -5, -0.5};
        light->color = glm::vec3(1.0, 0.2, 0.5); light->color *= 8;
        light->constant = 1.f;
        light->linear = 0.5f;
        light->quadratic = 0.8f;

        PointLight* light2 = render.addPointLight();
        light2->pos = {-5, -10, 3};
        light2->color = glm::vec3(0.3, 0.4, 0.9); light2->color *= 4;
        light2->constant = 1.f;
        light2->linear = 0.07f;
        light2->quadratic = 0.04f;


        shadowedLight = render.addPointLight();

        shadowedLight->pos = glm::vec3(20.5, -1, -1) + glm::vec3(3, 0, 0);
        shadowedLight->color = glm::vec3(2.8);//glm::vec3(1.0, 0.2, 0.5)*0.8f;
        shadowedLight->constant = 1.f;
        shadowedLight->linear = 0.1f;
        shadowedLight->quadratic = 0.03f;

        shadowedLight->setupShadow(1024, 100.f);
        shadowedLight->activateShadow();


        render.dirLight.color = glm::vec3(glm::vec2(0.5), 0.4) * 5.f;
        render.dirLight.dir = glm::vec3(0, -1, -1);

        render.dirLight.setupShadow({50, 50}, {7, 0, 0}, 100.f, {1024, 1024});
        render.dirLight.activateShadow();
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
        shadowedLight->pos = centr + curVec;

        if(Window::isPressed(GLFW_KEY_P))
            std::cout << "Camera pos: (" << cam.pos.x << ' ' << cam.pos.y << ' ' << cam.pos.z << "\n";
    }

    void draw()
    {
        render.draw(cam.getViewMatrix(), cam.getProjectionMatrix());
    }
}

