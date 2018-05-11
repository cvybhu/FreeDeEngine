#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Shader
{
public:
    GLuint program;

    Shader(){}
    Shader(const std::string path)  {load(path);}

    void load(const std::string path);

    void use(){glUseProgram(program);}

    template <class... Args> void set1Int(const char* name, Args... args)  {glUniform1i(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set2Int(const char* name, Args... args)  {glUniform2i(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set3Int(const char* name, Args... args)  {glUniform3i(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set4Int(const char* name, Args... args)  {glUniform4i(glGetUniformLocation(program, name), args...);}

    template <class... Args> void set1Float(const char* name, Args... args)  {glUniform1f(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set2Float(const char* name, Args... args)  {glUniform2f(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set3Float(const char* name, Args... args)  {glUniform3f(glGetUniformLocation(program, name), args...);}
    template <class... Args> void set4Float(const char* name, Args... args)  {glUniform4f(glGetUniformLocation(program, name), args...);}

    void setMat4(const char* name, const glm::mat4& theMat) { glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(theMat));}
};




