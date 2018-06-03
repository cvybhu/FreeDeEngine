#include <Window.hpp>
#include <glad/glad.h>
#include <Logger.hpp>

namespace Window
{
    GLFWwindow* window;
    int width = 1600, height = 900;
    glm::vec2 mousePos(width/2, height/2);
    glm::vec2 mouseDelta(0, 0);


    void update()
    {
        mouseDelta = {0, 0};
        glfwWaitEvents();
    }

    // -- Callbacks \/ --

    void mouse_callback(GLFWwindow* win, double xpos, double ypos)
    {
        glm::vec2 newMousePos(xpos, ypos);
        mouseDelta = newMousePos - mousePos;
        mousePos = newMousePos;
    }

    void framebuffer_size_callback(GLFWwindow* win, int newWidth, int newHeight)
    {
        width = newWidth;
        height = newHeight;
        glViewport(0, 0, newWidth, newHeight);
    }

    // -- Callbacks ^


    void init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
        #endif

        window = glfwCreateWindow(width, height, "xertz engine", NULL, NULL);
        glfwGetWindowSize(window, &width, &height);

        //glfwSetWindowPos(window, 200, 50);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window\n";
            glfwTerminate();
        }


        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);   //(VSYNC)

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD\n";
        }
        glViewport(0, 0, width, height);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  


        glfwSetCursorPos(window, mousePos.x, mousePos.y);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}
