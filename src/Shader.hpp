#pragma once

#include "glIncludes.hpp"

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    GLuint program;

    Shader(){}
    Shader(string vertexShaderPath, string fragmentShaderPath);

    void use(){glUseProgram(program);}

    template <class... Args> void set1Int(const std::string &name, Args... args)  {glUniform1i(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set2Int(const std::string &name, Args... args)  {glUniform2i(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set3Int(const std::string &name, Args... args)  {glUniform3i(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set4Int(const std::string &name, Args... args)  {glUniform4i(glGetUniformLocation(program, name.c_str()), args...);}

    template <class... Args> void set1Float(const std::string &name, Args... args)  {glUniform1f(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set2Float(const std::string &name, Args... args)  {glUniform2f(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set3Float(const std::string &name, Args... args)  {glUniform3f(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set4Float(const std::string &name, Args... args)  {glUniform4f(glGetUniformLocation(program, name.c_str()), args...);}

    void setMat4(const std::string& name, const glm::mat4& theMat) { glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(theMat));}




private:
    string readFile(const string& path) const
    {
        ifstream file(path);

        if(!file.is_open())
        {
            cout << "[ERROR] - failed to open " << path << "!\n";
            return "";
        }

        string result;

        string line;
        while(getline(file, line))
            result += (line + '\n');

        file.close();

        return result;
    }
};




