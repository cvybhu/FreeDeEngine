#include <Shader.hpp>

#include <string>
#include <fstream>
#include <Logger.hpp>
using namespace std;

string readFile(std::string&& path)
{
    ifstream file(path);

    if(!file.is_open())
    {
        Log << "[SHADER ERROR] - failed to open " << path << "!\n";
        return "";
    }

    string result;

    string line;
    while(getline(file, line))
            result += (line + '\n');

    file.close();

    return result;
}



void Shader::load(std::string path)
{
    //vertex shader
    string vertSource = readFile((path + ".vs"));
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
        Log << "[SHADER ERROR] - failed to compile shader " << (path + ".vs") << "!\n";
        Log << "log:\n" << infoLog << '\n';
        return;
    }


    //fragment shader
    string fragSource = readFile((path + ".fs"));
    const char* fragSourceCstr = fragSource.c_str();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSourceCstr, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        Log << "[SHADER ERROR] - failed to compile shader " << (path + ".fs") << "!\n";
        Log << "log:\n" << infoLog << '\n';
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
        Log << "[SHADER ERROR] - failed to link shader program for shader: " << path << "!\n";
        Log << "log:\n" << infoLog << '\n';
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Log << "[SHADERLOAD] succesfully loaded " << path << "!\n";
}

