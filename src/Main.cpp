#include <Window.hpp>
#include <Renderer.hpp>
#include <Game.hpp>
#include <Storage.hpp>
#include <Logger.hpp>
#include <TimeTeller.hpp>
#include <Utils.hpp>


using namespace std;




int main()
{
    //Initialization and asset loading
    double loadStartTime = glfwGetTime();

    Window::init();


    //const char* meshes2Load[] = {"mesh/spacePlane.obj", "mesh/light.obj", "mesh/grass.obj", "mesh/stonePlace.obj", "mesh/particle.obj", "mesh/pointShadowTest.obj"};
    const char* meshes2Load[] = {"mesh/lightBallForShading.obj", "mesh/pbrCube.obj", "mesh/light.obj", "mesh/floor.obj"};

    for(unsigned i = 0; i < sizeof(meshes2Load)/sizeof(const char*); i++)
    {
        Mesh& mesh = Storage::getMesh(meshes2Load[i]);
        mesh.loadToRAM(meshes2Load[i]);
        mesh.loadToGPU();
    }

    const char* shaders2Load[] = {"src/shaders/postProcess", "src/shaders/skybox", "src/shaders/dirLightShadow", "src/shaders/pointLightShadow", "src/shaders/showTBN", "src/shaders/gausBlur", "src/shaders/justColor", "src/shaders/deffered", "src/shaders/streched2cube", "src/shaders/diffRadGen", "src/shaders/prefilterGen", "src/shaders/brdfLUTGen", "src/shaders/IBL", "src/shaders/pointLight", "src/shaders/dirLight", "src/shaders/bloomSelect"};

    for(unsigned i = 0 ;i < sizeof(shaders2Load)/sizeof(const char*); i++)
    {
        Shader& shader = Storage::getShader(shaders2Load[i]);
        shader.load(shaders2Load[i]);
    }

    Game::init();

    std::cout<<"LOAD AND INIT TIME: "<< (glfwGetTime() - loadStartTime)*1000<<"ms\n";

    checkGlError();

    showOgl(GL_MAX_VERTEX_IMAGE_UNIFORMS);
    showOgl(GL_MAX_GEOMETRY_IMAGE_UNIFORMS);
    showOgl(GL_MAX_FRAGMENT_IMAGE_UNIFORMS);
    showOgl(GL_MAX_COMPUTE_IMAGE_UNIFORMS);



    //Game loop
    double lastFrameTime = glfwGetTime();
    double deltaTime = 0;


    TimeTeller frameTimeTeller("frame time", 1.5);
    TimeTeller renderTimeTeller("render time", 1.5);
    while (!glfwWindowShouldClose(Window::window))
    {
        deltaTime = glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();


        frameTimeTeller.startMeasuring();

        //input
        if(Window::isPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(Window::window, true);

        Game::update((float)deltaTime);


        //rendering
        renderTimeTeller.startMeasuring();

        Game::draw();

        checkGlError();

        glfwSwapBuffers(Window::window);

        renderTimeTeller.stopMeasuring();

        //Window update
        Window::update();


        frameTimeTeller.stopMeasuring();
        //frameTimeTeller.tell();
        renderTimeTeller.tell();
    }

    return 0;
}
