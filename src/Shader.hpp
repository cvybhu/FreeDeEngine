#pragma once

#include "glIncludes.hpp"

#include <string>
#include <iostream>
#include <fstream>
using namespace std;


class Shader
{
public:
    GLuint program;

    Shader(){}
    Shader(string vertexShaderPath, string fragmentShaderPath);

    void use(){glUseProgram(program);}

    template <class... Args> void set1Int(const std::string &name, Args... args) const {glUniform1i(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set2Int(const std::string &name, Args... args) const {glUniform2i(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set3Int(const std::string &name, Args... args) const {glUniform3i(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set4Int(const std::string &name, Args... args) const {glUniform4i(glGetUniformLocation(program, name.c_str()), args...);}

    template <class... Args> void set1Float(const std::string &name, Args... args) const {glUniform1f(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set2Float(const std::string &name, Args... args) const {glUniform2f(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set3Float(const std::string &name, Args... args) const {glUniform3f(glGetUniformLocation(program, name.c_str()), args...);}
    template <class... Args> void set4Float(const std::string &name, Args... args) const {glUniform4f(glGetUniformLocation(program, name.c_str()), args...);}




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




