#include <Window.hpp>
#include <glad/glad.h>
#include <Logger.hpp>
#include <vector>

namespace Window
{
    GLFWwindow* window;
    int width = 1920, height = 1080;
    glm::vec2 mousePos(width/2, height/2);
    glm::vec2 mouseDelta(0, 0);

    void setupCallbacks();

    void init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
        #endif

        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        window = glfwCreateWindow(mode->width, mode->height, "Xertz engine", monitor, NULL);
        glfwGetWindowSize(window, &width, &height);

        //window = glfwCreateWindow(width, height, "xertz engine", NULL, NULL);

        //glfwSetWindowPos(window, 200, 50);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window\n";
            glfwTerminate();
        }


        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);   //(VSYNC)

        setupCallbacks();

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


    enum struct keyState
    {
        clicked,
        repeated,
        released
    };

    std::ostream& operator<<(std::ostream& os, const keyState& kState){return (kState == keyState::clicked ? std::cout << "clicked" : kState == keyState::repeated ? std::cout << "repeated" : std::cout << "released");}

    std::vector<keyState> keyStates;

    void update()
    {
        mouseDelta = {0, 0};
        glfwPollEvents();

        //update keys
        for(int key = 0; key < (int)keyStates.size(); key++)
        {
            if(glfwGetKey(window, key) == GLFW_PRESS)
            {
                if(keyStates[key] == keyState::released)
                    keyStates[key] = keyState::clicked;
                else
                    keyStates[key] = keyState::repeated;
            }
            else
                keyStates[key] = keyState::released;
        }
    }

    // -- Callbacks \/ --

    template <int identifier, class... Args>
    class MulitpleCallbackCaller
    {
    public:
        static void call(Args... args)
        {
            for(auto callback : callbacks)
                callback(std::forward<Args>(args)...);
        }

        static void addCallback(void (*callbackPtr)(Args...))
            {callbacks.emplace_back(callbackPtr);}

    private:
        static std::vector<void(*)(Args...)> callbacks;
    };
    template <int identifier, class... Args> std::vector<void(*)(Args...)> MulitpleCallbackCaller<identifier, Args...>::callbacks;


    MulitpleCallbackCaller<0, GLFWwindow*, int, int, int, int>  keyCallbacks;
    MulitpleCallbackCaller<0, GLFWwindow*, unsigned>            charCallbacks;
    MulitpleCallbackCaller<0, GLFWwindow*, unsigned, int>       charModsCallbacks;
    MulitpleCallbackCaller<0, GLFWwindow*, double, double>      cursorPosCallbacks;
    MulitpleCallbackCaller<0, GLFWwindow*, int>                 cursorEnterCallbacks;
    MulitpleCallbackCaller<0, GLFWwindow*, int, int, int>       mouseButtonCallbacks;
    MulitpleCallbackCaller<1, GLFWwindow*, double, double>      scrollCallbacks;
    MulitpleCallbackCaller<0, int, int>            joystickCallbacks;
    MulitpleCallbackCaller<0, GLFWwindow*, int, const char**>   dropCallbacks;
    MulitpleCallbackCaller<0, GLFWwindow*, int, int>            framebuffSizeCallbacks;


    void addKeyCallback(            void (*funcPtr)(GLFWwindow* win, int key, int scancode, int action, int mods))  {keyCallbacks.addCallback(funcPtr);}
    void addCharCallback(           void (*funcPtr)(GLFWwindow* win, unsigned int codepoint))                       {charCallbacks.addCallback(funcPtr);}
    void addCharModsCallback(       void (*funcPtr)(GLFWwindow* win, unsigned int codepoint, int mods))             {charModsCallbacks.addCallback(funcPtr);}
    void addCursorPosCallback(      void (*funcPtr)(GLFWwindow* win, double xpos, double ypos))                     {cursorPosCallbacks.addCallback(funcPtr);}
    void addCursorEnterCallback(    void (*funcPtr)(GLFWwindow* win, int entered))                                  {cursorEnterCallbacks.addCallback(funcPtr);}
    void addMouseButtonCallback(    void (*funcPtr)(GLFWwindow* win, int button, int action, int mods))             {mouseButtonCallbacks.addCallback(funcPtr);}
    void addScrollCallback(         void (*funcPtr)(GLFWwindow* win, double xoffset, double yoffset))               {scrollCallbacks.addCallback(funcPtr);}
    void addJoystickCallback(       void (*funcPtr)(int joy, int event))                                            {joystickCallbacks.addCallback(funcPtr);}
    void addDropCallback(           void (*funcPtr)(GLFWwindow* window, int count, const char** paths))             {dropCallbacks.addCallback(funcPtr);}
    void addFramebufferSizeCallback(void (*funcPtr)(GLFWwindow* win, int newWidth, int newHeight))                  {framebuffSizeCallbacks.addCallback(funcPtr);}


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

    void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods)
    {
        if(key >= (int)keyStates.size())
                keyStates.resize(key+1, keyState::released);
    }

    void setupCallbacks()
    {
        glfwSetKeyCallback(window, keyCallbacks.call);
        glfwSetCharCallback(window, charCallbacks.call);
        glfwSetCharModsCallback(window, charModsCallbacks.call);
        glfwSetCursorPosCallback(window, cursorPosCallbacks.call);
        glfwSetCursorEnterCallback(window, cursorEnterCallbacks.call);
        glfwSetMouseButtonCallback(window, mouseButtonCallbacks.call);
        glfwSetScrollCallback(window, scrollCallbacks.call);
        glfwSetJoystickCallback(joystickCallbacks.call);
        glfwSetDropCallback(window, dropCallbacks.call);
        glfwSetFramebufferSizeCallback(window, framebuffSizeCallbacks.call);

        framebuffSizeCallbacks.addCallback(framebuffer_size_callback);
        cursorPosCallbacks.addCallback(mouse_callback);
        keyCallbacks.addCallback(key_callback);
    }


    // -- Callbacks ^

    bool isPressed(int keyCode)
    {
        return ((int)keyStates.size() > keyCode && keyStates[keyCode] != keyState::released);
    }

    bool isClicked(int keyCode)
    {
        return ((int)keyStates.size() > keyCode && keyStates[keyCode] == keyState::clicked);
    }
}
