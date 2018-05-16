#include <Render.hpp>
#include <Utils.hpp>
#include <iostream>
#include <fstream>
#include <map>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


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

        width = 1600;
        height = 900;

        window = glfwCreateWindow(width, height, "xertz engine", NULL, NULL);
        glfwGetWindowSize(window, &width, &height);
        //glfwSetWindowPos(window, 200, 50);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window\n";
            glfwTerminate();
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD\n";
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);


        glfwSetCursorPos(window, mousePos.x, mousePos.y);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}



void Texture::loadToRAM(const char* filePath)
{
    stbi_set_flip_vertically_on_load(true);
    //stbi_set_flip_horizontally_on_load(true);
    data = stbi_load(filePath, &width, &height, &nrChannels, 0);

    if (!data)
    {
        std::cout << "[ERROR TEX] Failed to load texture " << filePath << "\n";
    }

    isOnRAM = true;

    std::cout << "[TEXLOAD]Succesfully loaded " << filePath << "!\n";
}



void Texture::loadToGPU()
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_2D, glIndx);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(nrChannels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    isOnGPU = true;
}


void Texture::unloadFromRAM()
{
    stbi_image_free(data);

    isOnRAM = false;
}

void Texture::unloadFromGPU()
{
    //TODO
    isOnGPU = false;
}

unsigned char* Texture::getPixel(const int& x, const int& y){return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}
const unsigned char* Texture::getPixel(const int& x, const int& y) const {return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}



void CubeTexture::loadToRAM(const char* fileName)
{
    int  nrChannels;
    stbi_set_flip_vertically_on_load(true);
    data[0] = stbi_load((std::string(fileName) + "_left.jpg").c_str(), &width, &height, &nrChannels, 0); //X+
    data[1] = stbi_load((std::string(fileName) + "_right.jpg").c_str(), &width, &height, &nrChannels, 0);  //X-
    data[2] = stbi_load((std::string(fileName) + "_back.jpg").c_str(), &width, &height, &nrChannels, 0);    //Y+
    data[3] = stbi_load((std::string(fileName) + "_front.jpg").c_str(), &width, &height, &nrChannels, 0);  //Y-
    data[4] = stbi_load((std::string(fileName) + "_up.jpg").c_str(), &width, &height, &nrChannels, 0);  //Z+
    data[5] = stbi_load((std::string(fileName) + "_down.jpg").c_str(), &width, &height, &nrChannels, 0); //Z-

    for(int i = 0; i < 6; i++)
        if(!data[i])
        {
            std::cout << "[TEXLOAD] Failed to load " << fileName << "!!!!!!!!!!!!!!\n";
        }

    isOnRAM = true;
}

void CubeTexture::loadToGPU()
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glIndx);

    for(int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    isOnGPU = true;
}


void CubeTexture::unloadFromRAM()
{
    for(int i = 0; i < 6; i++)
        stbi_image_free(data[i]);

    isOnRAM = false;
}

void CubeTexture::unloadFromGPU()
{
    //TODO

    isOnGPU = false;
}





void Shader::load(const std::string& path)
{
    //vertex shader
    std::string vertSource = readFile((path + ".vs").c_str());
    const char* vertSourceCstr = vertSource.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertSourceCstr, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "[SHADER ERROR] - failed to compile shader " << (path + ".vs") << "!\n";
        std::cout << "log:\n" << infoLog << '\n';
        return;
    }


    //fragment shader
    std::string fragSource = readFile((path + ".fs").c_str());
    const char* fragSourceCstr = fragSource.c_str();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSourceCstr, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "[SHADER ERROR] - failed to compile shader " << (path + ".fs") << "!\n";
        std::cout << "log:\n" << infoLog << '\n';
        return;
    }


    //the Program
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "[SHADER ERROR] - failed to link shader program for shader: " << path << "!\n";
        std::cout << "log:\n" << infoLog << '\n';
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "[SHADERLOAD]Succesfully loaded " << path << "!\n";
}


void Mesh::loadToRAM(const char* filePath)
{
    std::ifstream file(filePath);

    std::string input;

    std::vector<glm::vec3> vecPoses;
    std::vector<glm::vec3> vecNormals;
    std::vector<glm::vec2> texCoords;

    specTexture = nullptr;

    while(file >> input)
    {
        if(input != "v" && input != "vt" && input != "vn" && input != "f" && input != "diffTex" && input != "specTex")
            continue;

        if(input == "v")
        {
            glm::vec3 pos;
            file >> pos.x >> pos.y >> pos.z;
            vecPoses.emplace_back(pos);
            continue;
        }

        if(input == "vn")
        {
            glm::vec3 normal;
            file >> normal.x >> normal.y >> normal.z;
            vecNormals.emplace_back(normal);
            continue;
        }


        if(input == "vt")
        {
            glm::vec2 coord;
            file >> coord.x >> coord.y;
            texCoords.emplace_back(coord);
            continue;
        }

        if(input == "f")
        {
            for(int i = 0; i < 3;i++)
            {
                Vertex v;
                int index; char spacer;
                file >> index; file >> spacer;
                v.pos = vecPoses[index-1];
                file >> index; file >> spacer;
                v.texCoords = texCoords[index-1];
                file >> index;
                v.normal = vecNormals[index-1];

                verts.emplace_back(v);
            }

            continue;
        }

        if(input == "diffTex")
        {
            std::string texPath;
            file >> texPath;

            diffTexture = &Storage::getTex(texPath.c_str());

            if(!diffTexture->isOnRAM)
                diffTexture->loadToRAM(texPath.c_str());

            continue;
        }

        if(input == "specTex")
        {
            std::string texPath;
            file >> texPath;

            specTexture = &Storage::getTex(texPath.c_str());

            if(!specTexture->isOnRAM)
                specTexture->loadToRAM(texPath.c_str());

            continue;
        }
    }

    if(specTexture == nullptr)
        specTexture = diffTexture;

    file.close();

    std::cout << "[MESHLOAD]Succesfully loaded " << filePath << " (" << verts.size() << " vertices)\n";

    vertsNum = verts.size();
}

void Mesh::unloadFromRAM()
{
    verts.clear(); verts.shrink_to_fit();
}


void Mesh::loadToGPU()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),&indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);

    if(!diffTexture->isOnGPU)
        diffTexture->loadToGPU();

    if(!specTexture->isOnGPU)
        specTexture->loadToGPU();
}

void Mesh::unloadFromGPU()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}






FreeCam::FreeCam(glm::vec3 position, glm::vec2 rotation, float Speed)
{
    pos = position;
    rot = rotation;
    speed = Speed;
    fov = 45.0f;
    sensitivity = 0.003;
}


void FreeCam::moveForward(float deltaTime)
{
    pos += speed*deltaTime*glm::rotate(glm::vec3(0, -1, 0), rot[0], glm::vec3(0, 0, 1));
}

void FreeCam::moveBack(float deltaTime)
{
    moveForward(-deltaTime);
}


void FreeCam::moveRight(float deltaTime)
{
    pos += speed*deltaTime*glm::rotate(glm::vec3(-1, 0, 0), rot[0], glm::vec3(0, 0, 1));
}

void FreeCam::moveLeft(float deltaTime)
{
    moveRight(-deltaTime);
}

void FreeCam::moveUp(float deltaTime)
{
    pos.z += speed*deltaTime;
}

void FreeCam::moveDown(float deltaTime)
{
    moveUp(-deltaTime);
}


void FreeCam::handleCameraRot(glm::vec2 mouseDelta)
{
    rot[0] -= mouseDelta.x*sensitivity;
    rot[1] += mouseDelta.y*sensitivity;

    float almost90 = glm::radians(89.5);
    if(rot[1] < -almost90)rot[1] = -almost90;
    if(rot[1] > almost90)rot[1] = almost90;
}



void FreeCam::handleMovement(float deltaTime)
{
    if(Window::isPressed(GLFW_KEY_W))moveForward(deltaTime);
    if(Window::isPressed(GLFW_KEY_S))moveBack(deltaTime);
    if(Window::isPressed(GLFW_KEY_A))moveLeft(deltaTime);
    if(Window::isPressed(GLFW_KEY_D))moveRight(deltaTime);
    if(Window::isPressed(GLFW_KEY_SPACE))moveUp(deltaTime);
    if(Window::isPressed(GLFW_KEY_LEFT_SHIFT))moveDown(deltaTime);

    handleCameraRot(Window::mouseDelta);
}


glm::mat4 FreeCam::getViewMatrix()
{
    glm::vec3 up(0, 0, 1);
    glm::vec3 front(0, -1, 0);
    glm::vec3 right = glm::cross(up, front);

    front = glm::rotate(front, rot[0], up); //yaw
    right = glm::rotate(right, rot[0], up);

    front = glm::rotate(front, rot[1], right); //pitch
    up = glm::rotate(up, rot[1], right);

    return glm::lookAt(pos, pos + front, up);
}


glm::mat4 FreeCam::getProjectionMatrix()
{
    return glm::perspective(glm::radians(fov), (float)Window::width / (float)Window::height, 0.1f, 500.0f);
}





//Storage stuff - plz keep at the end of the file
namespace Storage
{
    std::map<std::string, Shader> shaders;
    std::map<std::string, Texture> textures;
    std::map<std::string, Mesh> meshes;
    std::map<std::string, CubeTexture> cubeTextures;


    Shader& getShader(const char* filePath)
    {
        return shaders[filePath];
    }


    Texture& getTex(const char* filePath)
    {
        return textures[filePath];
    }


    Mesh& getMesh(const char* filePath)
    {
        return meshes[filePath];
    }

    CubeTexture& getCubeTex(const char* fileName)
    {
        return cubeTextures[fileName];
    }
}
