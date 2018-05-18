#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <map>
#include <iomanip>
#include <string>
#include <vector>



//Utils
class TimeTeller    //TimeTeller is used to measure time multiple times and tell max measured time from every <cooldown> seconds
{                   //tell() only "tells" max time when cooldown wears off
public:
    TimeTeller(const char* Name, double TellCooldown);

    void startMeasuring();
    void stopMeasuring();

    void tell();
private:
    const char* name;
    double startTime;
    const double tellCooldown;
    double lastTellTime;
    double maxTime;
};



std::string readFile(const char* path); //puts file in std::string




//string conversion funcs
int convert2Int(const char*);
long long convert2LL(const char*);
float convert2Float(const char*);
double convert2Double(const char*);
unsigned convert2Uint(const char*);
unsigned long long convert2ULL(const char*);

int convert2Int(const std::string&);
long long convert2LL(const std::string&);
float convert2Float(const std::string&);
double convert2Double(const std::string&);
unsigned convert2Uint(const std::string&);
unsigned long long convert2ULL(std::string&);


//these write to place given in str and return place after the writing
char* convert2Cstring(const int&, char* str);
char* convert2Cstring(const long long&, char* str);
char* convert2Cstring(const float&, char* str);
char* convert2Cstring(const double&, char* str);
char* convert2Cstring(const unsigned&, char* str);
char* convert2Cstring(const unsigned long long&, char* str);


std::string convert2String(const int&);
std::string convert2String(const long long&);
std::string convert2String(const float&);
std::string convert2String(const double&);
std::string convert2String(const unsigned&);
std::string convert2String(const unsigned long long&);




namespace Window
{
    extern GLFWwindow* window;
    extern int height, width;

    extern glm::vec2 mousePos;
    extern glm::vec2 mouseDelta;

    void init();
    inline bool isPressed(int keyCode){return (glfwGetKey(window, keyCode) == GLFW_PRESS);}

    void update();
}




//Render
struct Texture //holds pixel data. possible to load from file and on GPU
{
    unsigned char* data;    //data is stored as 1D array (R,G,B,A,X,  R,G,B,A,X, etc...)
    int width, height;
    int nrChannels;         //R G B is 3 channels, with alpha its 4 etc.

    GLuint glIndx;

    bool isOnRAM, isOnGPU;

    void loadToRAM(const char* filePath);
    void loadToGPU(bool fixGamma = false);

    void unloadFromRAM();
    void unloadFromGPU();

    unsigned char* getPixel(const int& x, const int& y);              //x goes L->R y goes D->U thats kinda prototype
    const unsigned char* getPixel(const int& x, const int& y) const;  //func gives pointer to place where pixel begins
};

namespace Storage  //all Textures are held in global Storage and can be accesed via Storage::getTex("<filePath>")
{
    Texture& getTex(const char* filePath);
}


struct CubeTexture
{
    unsigned char* data[6];
    int width, height;

    GLuint glIndx;

    bool isOnRAM, isOnGPU;

    void loadToRAM(const char* fileName);    //its expected that cubemap is like "fileName_up.jpg", _down, _front _back _right _left
    void loadToGPU(bool fixGamma = false);                       //also all of the files should have same resolution

    void unloadFromRAM();
    void unloadFromGPU();
};

namespace Storage   //all CubeTextures are held in global Storage and can be accesed via Storage::getCubeTex("<fileName>")
{
    CubeTexture& getCubeTex(const char* fileName);
}




struct Shader   //loads and compiles openGL shader. has cool methods to send data to uniforms
{
    GLuint program;

    void load(const std::string& path);

    void use(){glUseProgram(program);}

    void set1Int(const char* name, int a)                       {glUniform1i(glGetUniformLocation(program, name), a);}
    void set2Int(const char* name, int a, int b)                {glUniform2i(glGetUniformLocation(program, name), a, b);}
    void set3Int(const char* name, int a, int b, int c)         {glUniform3i(glGetUniformLocation(program, name), a, b, c);}
    void set4Int(const char* name, int a, int b, int c, int d)  {glUniform4i(glGetUniformLocation(program, name), a, b, c, d);}


    void set1Float(const char* name, float a)                             {glUniform1f(glGetUniformLocation(program, name), a);}
    void set2Float(const char* name, float a, float b)                    {glUniform2f(glGetUniformLocation(program, name), a, b);}
    void set3Float(const char* name, float a, float b, float c)           {glUniform3f(glGetUniformLocation(program, name), a, b, c);}
    void set4Float(const char* name, float a, float b, float c, float d)  {glUniform4f(glGetUniformLocation(program, name), a, b, c, d);}

    void setVec2(const char* name, glm::vec2 vec) {glUniform2f(glGetUniformLocation(program, name), vec.x, vec.y);}
    void setVec3(const char* name, glm::vec3 vec) {glUniform3f(glGetUniformLocation(program, name), vec.x, vec.y, vec.z);}
    void setVec4(const char* name, glm::vec4 vec) {glUniform4f(glGetUniformLocation(program, name), vec.x, vec.y, vec.z, vec.w);}

    void setMat4(const char* name, const glm::mat4& theMat) { glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(theMat));}
};

namespace Storage    //all Shaders are held in global Storage and can be accesed via Storage::getShader("<filePath>")
{
    Shader& getShader(const char* filePath);
}




struct Mesh //holds vertex data and pointers to used textures from global storage. Loadable from file and on GPU
{
    void loadToRAM(const char* filePath);
    void loadToGPU();

    void unloadFromRAM();
    void unloadFromGPU();

    Texture* diffTexture;
    Texture* specTexture;

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    std::vector<Vertex> verts;
    int vertsNum;
    GLuint VBO, VAO;
};

namespace Storage  //all Meshes are held in global Storage and can be accesed via Storage::getMesh("<filePath>")
{
    Mesh& getMesh(const char* filePath);
}


struct FreeCam
{
    FreeCam(glm::vec3 position = {0, 0, 0}, glm::vec2 rotation = {0, 0}, float speed = 10);

    glm::vec3 pos;
    glm::vec2 rot;  //yaw pitch (in radians)

    float fov;  //in degrees

    float sensitivity;
    float speed;


    void handleCameraRot(glm::vec2 mouseDelta);
    void handleMovement(float deltaTime);

    void moveForward(float deltaTime);
    void moveBack(float deltaTime);
    void moveRight(float deltaTime);
    void moveLeft(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);


//openGL stuff
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
};


struct PointLight
{
    glm::vec3 pos;

    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;
};



struct DirLight
{
    glm::vec3 color;

    glm::vec3 dir;
};



//Game
namespace Game
{
    void init();

    void update(const GLdouble deltaTime);
    void draw();
}



//HERE HEADERS END

//Utilscpp
TimeTeller::TimeTeller(const char*Name, double TellCooldown) : tellCooldown(TellCooldown)
{
    maxTime = -1;
    name = Name;
    lastTellTime = glfwGetTime();
}

void TimeTeller::startMeasuring()
{
    startTime = glfwGetTime();
}

void TimeTeller::stopMeasuring()
{
    double curTime = glfwGetTime() - startTime;

    if(maxTime < 0)
    {
        maxTime = curTime;
    }
    else
    {
        if(curTime > maxTime)
            maxTime = curTime;
    }
}


void TimeTeller::tell()
{
    if(glfwGetTime() - lastTellTime >= tellCooldown)
    {
        std::cout << name << std::setprecision(5) << std::fixed << ": " << maxTime*1000.0 << "ms\n";
        lastTellTime = glfwGetTime();
        maxTime = -1;
    }
}


std::string readFile(const char* path)
{
    std::ifstream file(path);

    if(!file.is_open())
    {
        std::cout << "failed to open " << path << "!\n";
        return "";
    }

    std::string result;

    std::string line;
    while(getline(file, line))
            result += (line + '\n');

    file.close();

    return result;
}





//string convertion funcs
int convert2Int(const char* str)        {int res;      std::istringstream(str) >> res; return res;}
long long convert2LL(const char* str)   {long long res; std::istringstream(str) >> res; return res;}
float convert2Float(const char* str)    {float res;    std::istringstream(str) >> res; return res;}
double convert2Double(const char* str)  {double res;   std::istringstream(str) >> res; return res;}
unsigned convert2Uint(const char* str)  {unsigned res; std::istringstream(str) >> res; return res;}
unsigned long long convert2ULL(const char* str) {unsigned long long res; std::istringstream(str) >> res; return res;}

int convert2Int(const std::string& str)         {int res;      std::istringstream(str) >> res; return res;}
long long convert2LL(const std::string& str)    {long long res; std::istringstream(str) >> res; return res;}
float convert2Float(const std::string& str)     {float res;    std::istringstream(str) >> res; return res;}
double convert2Double(const std::string& str)   {double res;   std::istringstream(str) >> res; return res;}
unsigned convert2Uint(const std::string& str)   {unsigned res;   std::istringstream(str) >> res; return res;}
unsigned long long convert2ULL(std::string& str){unsigned long long res;   std::istringstream(str) >> res; return res;}


char* convert2Cstring(const int& a, char* str)       {std::string s = convert2String(a); for(char c : s)*(str++) = c; return str;}
char* convert2Cstring(const long long& a, char* str) {std::string s = convert2String(a); for(char c : s)*(str++) = c; return str;}
char* convert2Cstring(const float& a, char* str)     {std::string s = convert2String(a); for(char c : s)*(str++) = c; return str;}
char* convert2Cstring(const double& a, char* str)    {std::string s = convert2String(a); for(char c : s)*(str++) = c; return str;}
char* convert2Cstring(const unsigned& a, char* str)  {std::string s = convert2String(a); for(char c : s)*(str++) = c; return str;}
char* convert2Cstring(const unsigned long long& a, char* str) {std::string s = convert2String(a); for(char c : s)*(str++) = c; return str;}


std::string convert2String(const int& a){std::stringstream s; s << a; std::string res; s >> res; return res;}
std::string convert2String(const long long& a){std::stringstream s; s << a; std::string res; s >> res; return res;}
std::string convert2String(const float& a){std::stringstream s; s << a; std::string res; s >> res; return res;}
std::string convert2String(const double& a){std::stringstream s; s << a; std::string res; s >> res; return res;}
std::string convert2String(const unsigned& a){std::stringstream s; s << a; std::string res; s >> res; return res;}
std::string convert2String(const unsigned long long& a){std::stringstream s; s << a; std::string res; s >> res; return res;}


//Rendercpp

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
        glViewport(0, 0, width, height);

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



void Texture::loadToGPU(bool fixGamma)
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_2D, glIndx);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(nrChannels == 3)
    {
        if(fixGamma)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        if(fixGamma)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

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

void CubeTexture::loadToGPU(bool fixGamma)
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glIndx);

    for(int i = 0; i < 6; i++)
        if(fixGamma)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
        else
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


    //geometry shader
    std::ifstream geoShaderFile(path + ".gs");

    bool isGeometryShader = geoShaderFile.is_open();

    GLuint geoShader = -1;
    if(isGeometryShader)
    {
        std::string geoSource = readFile((path + ".gs").c_str());
        const char* geoSourceCstr = geoSource.c_str();

        geoShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geoShader, 1, &geoSourceCstr, NULL);
        glCompileShader(geoShader);

        glGetShaderiv(geoShader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(geoShader, 512, NULL, infoLog);
            std::cout << "[SHADER ERROR] - failed to compile shader " << (path + ".gs") << "!\n";
            std::cout << "log:\n" << infoLog << '\n';
            return;
        }
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

    if(isGeometryShader)
        glAttachShader(program, geoShader);

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

    if(isGeometryShader)
        glDeleteShader(geoShader);

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
        diffTexture->loadToGPU(true);

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


//Storage stuff
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

//Gamecpp

namespace Game
{
    std::vector<PointLight> pointLights;
    DirLight sun;

    FreeCam cam;


    GLuint framebuffer;
    GLuint texColorBuffer;
    GLuint depthStencRenderBuff;
    GLuint screenQuadVAO, screenQuadVBO;

    GLuint skyboxIndex;
    GLuint skyboxVAO, skyboxVBO;


    void initFramebuffer()
    {
        glEnable(GL_MULTISAMPLE);

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texColorBuffer);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, Window::width, Window::height, GL_TRUE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glBindTexture(GL_TEXTURE_2D, 0); //is this really necessary?

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texColorBuffer, 0);

        glGenRenderbuffers(1, &depthStencRenderBuff);
        glBindRenderbuffer(GL_RENDERBUFFER, depthStencRenderBuff);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, Window::width, Window::height);
        //glBindRenderbuffer(GL_RENDERBUFFER, 0); //is this really necessary?

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencRenderBuff);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Failed to setup framebuffer!!!\n";

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        float screenQuadVerts[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &screenQuadVAO);
        glGenBuffers(1, &screenQuadVBO);
        glBindVertexArray(screenQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVerts), &screenQuadVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }


    void initSkyboxVAO()
    {
        float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    void initInstancedParticles()
    {
        glm::vec3 poss[] = {glm::vec3(0, 13, 0), glm::vec3(0, 14, 0), glm::vec3(0, 15, 0), glm::vec3(0, 16, 0), glm::vec3(0, 17, 0)};
        std::vector<glm::mat4> modelMats(sizeof(poss)/sizeof(glm::vec3));

        for(int i = 0; i < modelMats.size(); i++)
            modelMats[i] = glm::translate(glm::mat4(1), poss[i]);

        Mesh& particle = Storage::getMesh("mesh/particle.obj");
        glBindVertexArray(particle.VAO);

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, modelMats.size() * sizeof(glm::mat4), &modelMats[0], GL_STATIC_DRAW);
        // vertex Attributes
        GLsizei vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }


    void init()
    {
        initFramebuffer();
        initSkyboxVAO();
        initInstancedParticles();

        CubeTexture& skyboxMountLake = Storage::getCubeTex("src/tex/mountainsCube");

        skyboxMountLake.loadToRAM("tex/mountainsCube");
        skyboxMountLake.loadToGPU(true);

        skyboxIndex = skyboxMountLake.glIndx;

        sun.color = glm::vec3(glm::vec2(0.3), 0);
        sun.dir = {0, -1.0f, -1.0f};


        PointLight light;
        light.pos = {5, -5, 2};
        light.color = glm::vec3(1.0, 0.2, 0.5);
        light.constant = 1.f;
        light.linear = 0.05f;
        light.quadratic = 0.004f;

        PointLight light2;
        light2.pos = {-5, -10, 3};
        light2.color = glm::vec3(0.3, 0.4, 0.9);
        light2.constant = 1.f;
        light2.linear = 0.07f;
        light2.quadratic = 0.004f;

        pointLights.emplace_back(light); pointLights.emplace_back(light2);
    }

    void update(const GLdouble deltaTime)
    {
        cam.handleMovement(deltaTime);


        if(Window::isPressed(GLFW_KEY_P))
            std::cout << "Camera pos: (" << cam.pos.x << ' ' << cam.pos.y << ' ' << cam.pos.z << "\n";
    }


    void loadPointLightsToShader(std::vector<PointLight>& lights, Shader& shader)
    {
        shader.set1Int("pointLightsNum", (int)lights.size());

        for(unsigned i = 0; i < lights.size(); i++)
        {
            auto& light = lights[i];
            std::string&& iAsString = convert2String(i);

            shader.setVec3(("pointLights[" + iAsString + "].pos").c_str(), light.pos);
            shader.setVec3(("pointLights[" + iAsString + "].color").c_str(), light.color);
            shader.set1Float(("pointLights[" + iAsString + "].constant").c_str(), light.constant);
            shader.set1Float(("pointLights[" + iAsString + "].linear").c_str(), light.linear);
            shader.set1Float(("pointLights[" + iAsString + "].quadratic").c_str(), light.quadratic);
        }
    }

    /*
    void loadDirLightsToShader(std::vector<DirLight>& lights, Shader& shader)
    {
        shader.set1Int("dirLightsNum", lights.size());

        for(unsigned i = 0; i < lights.size(); i++)
        {
            auto& light = lights[i];
            std::string&& iAsString = convert2String(i);

            shader.setVec3(("dirLights[" + iAsString + "].color").c_str(), light.color);
            shader.setVec3(("dirLights[" + iAsString + "].dir").c_str(), light.dir);
        }
    }
    */

    void drawLights(std::vector<PointLight>& lights, glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
    {
        Shader& oneColor = Storage::getShader("src/shaders/oneColor");

        oneColor.use();

        oneColor.setMat4("projection", projectionMatrix);
        oneColor.setMat4("view", viewMatrix);

        Mesh& lightMesh = Storage::getMesh("mesh/light.obj");
        glBindVertexArray(lightMesh.VAO);

        for(PointLight& light : lights)
        {
            oneColor.setVec3("color", glm::normalize(light.color));
            oneColor.setMat4("model", glm::translate(glm::mat4(1), light.pos));
            glDrawArrays(GL_TRIANGLES, 0, lightMesh.vertsNum);
        }

    }

    void drawSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
    {
        Shader& skyboxShader = Storage::getShader("src/shaders/skybox");

        glDepthMask(GL_FALSE);
        skyboxShader.use();

        skyboxShader.setMat4("view", glm::mat3(viewMatrix));
        skyboxShader.setMat4("projection", projectionMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIndex);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

    }

    void setupMeshForDraw(const Mesh& mesh)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.diffTexture->glIndx);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh.specTexture->glIndx);
        glBindVertexArray(mesh.VAO);
    }

    void drawMesh(const Mesh& mesh, const glm::mat4& modelMatrix, Shader& shader)
    {
        shader.setMat4("model", modelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertsNum);
    }

    void draw()
    {
        glm::mat4 projectionMatrix = cam.getProjectionMatrix();
        glm::mat4 viewMatrix = cam.getViewMatrix();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClearColor(1, 1, 1, 1);

        drawSkybox(viewMatrix, projectionMatrix);


        Shader& lightUseShader = Storage::getShader("src/shaders/lightUse");

        lightUseShader.use();

        loadPointLightsToShader(pointLights, lightUseShader);
        //loadDirLightsToShader(dirLights, lightUseShader);
        lightUseShader.setVec3("dirLight.color", sun.color);
        lightUseShader.setVec3("dirLight.dir", sun.dir);

        lightUseShader.setMat4("view", viewMatrix);
        lightUseShader.setMat4("projection", projectionMatrix);

        lightUseShader.setVec3("ambientLight", glm::vec3(0.3));
        lightUseShader.setVec3("viewPos", cam.pos);

        lightUseShader.set1Int("diffTexture", 0);
        lightUseShader.set1Int("specTexture", 1);


        Mesh& planeMesh = Storage::getMesh("mesh/spacePlane.obj");
        setupMeshForDraw(planeMesh);
        drawMesh(planeMesh, glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(0, 0, 1)), lightUseShader);

        Mesh& stonePlace = Storage::getMesh("mesh/stonePlace.obj");
        setupMeshForDraw(stonePlace);
        glm::mat4 stonePlaceModel = glm::translate(glm::mat4(1), glm::vec3(0, -12, 0));
        drawMesh(stonePlace, stonePlaceModel, lightUseShader);

        Mesh& grass = Storage::getMesh("mesh/grass.obj");
        setupMeshForDraw(grass);
        glDisable(GL_CULL_FACE);
        for(glm::vec3 pos : {glm::vec3(-3, -8, -1), glm::vec3(-2, -7, -1), glm::vec3(-4, -6.3, -1)})
            drawMesh(grass, glm::translate(glm::mat4(1), pos), lightUseShader);
        glEnable(GL_CULL_FACE);





        Shader& instanceShader = Storage::getShader("src/shaders/instance");

        Mesh& particle = Storage::getMesh("mesh/particle.obj");

        instanceShader.use();
        instanceShader.setMat4("view", viewMatrix);
        instanceShader.setMat4("projection", projectionMatrix);

        setupMeshForDraw(particle);

        glDrawArraysInstanced(GL_TRIANGLES, 0, particle.vertsNum, 5);




        drawLights(pointLights, projectionMatrix, viewMatrix);

        Shader& postProcess = Storage::getShader("src/shaders/postProcessTest");

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClear(GL_COLOR_BUFFER_BIT);

        postProcess.use();
        glBindVertexArray(screenQuadVAO);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}


using namespace std;

int main()
{
    //Initialization and asset loading
    double loadStartTime = glfwGetTime();

    Window::init();


    const char* meshes2Load[] = {"mesh/spacePlane.obj", "mesh/light.obj", "mesh/grass.obj", "mesh/stonePlace.obj", "mesh/particle.obj"};

    for(unsigned i = 0; i < sizeof(meshes2Load)/sizeof(const char*); i++)
    {
        Mesh& mesh = Storage::getMesh(meshes2Load[i]);
        mesh.loadToRAM(meshes2Load[i]);
        mesh.loadToGPU();
    }


    const char* shaders2Load[] = {"src/shaders/lightUse", "src/shaders/oneColor", "src/shaders/postProcessTest", "src/shaders/skybox", "src/shaders/instance"};

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
