#include <iostream>
#include <Utils.hpp>
#include <Render.hpp>
#include <Game.hpp>

using namespace std;




int main()
{
    //Initialization and asset loading
    double loadStartTime = glfwGetTime();

    Window::init();


    const char* meshes2Load[] = {"mesh/spacePlane.obj", "mesh/light.obj"};

    for(unsigned i = 0; i < sizeof(meshes2Load)/sizeof(const char*); i++)
    {
        Mesh& mesh = Storage::getMesh(meshes2Load[i]);
        mesh.loadToRAM(meshes2Load[i]);
        mesh.loadToGPU();
    }


    const char* shaders2Load[] = {"src/shaders/light", "src/shaders/allWhite"};

    for(unsigned i = 0 ;i < sizeof(shaders2Load)/sizeof(const char*); i++)
    {
        Shader& shader = Storage::getShader(shaders2Load[i]);
        shader.load(shaders2Load[i]);
    }


    Game::init();

    std::cout<<"LOAD AND INIT TIME: "<< (glfwGetTime() - loadStartTime)*1000<<"ms\n";




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

        Game::update(deltaTime);





        //rendering
        renderTimeTeller.startMeasuring();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Game::draw();

        renderTimeTeller.stopMeasuring();


        frameTimeTeller.stopMeasuring();
        frameTimeTeller.tell();
        renderTimeTeller.tell();


        //Window updates
        Window::update();
        glfwSwapBuffers(Window::window);
    }




    return 0;
}

