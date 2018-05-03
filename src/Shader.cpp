#include "Shader.hpp"




Shader::Shader(string vertexShaderPath, string fragmentShaderPath)
{
    //vertex shader
    string vertSource = readFile(vertexShaderPath);
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
        cout << "[ERROR] - failed to compile shader " << vertexShaderPath << "!\n";
        cout << "log:\n" << infoLog << '\n';
        return;
    }


    //fragment shader
    string fragSource = readFile(fragmentShaderPath);
    const char* fragSourceCstr = fragSource.c_str();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSourceCstr, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "[ERROR] - failed to compile shader " << fragmentShaderPath << "!\n";
        cout << "log:\n" << infoLog << '\n';
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
        cout << "[ERROR] - failed to link shader program for shader: " << vertexShaderPath << "!\n";
        cout << "log:\n" << infoLog << '\n';
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

