#include <Shader.hpp>

#include <fstream>
#include <string>
#include <Utils.hpp>

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