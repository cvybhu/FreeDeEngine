#pragma once

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

struct Shader   //loads and compiles openGL shader. has cool methods to send data to uniforms
{
    GLuint program;

    void load(const std::string& path);

    bool isLoaded;

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

    void setUBO(const char* name, GLuint uboBindingPoint) { glUniformBlockBinding(program, glGetUniformBlockIndex(program, name), uboBindingPoint);}


};