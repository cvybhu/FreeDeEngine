#pragma once

#include <string>
#include <glm/vec3.hpp>
#include <iostream>
#include <assert.h>

#define showOgl(a) GLint xDD##a; glGetIntegerv(a, &xDD##a); std::cout << #a << ": " << xDD##a << '\n';
#define checkGlError() {GLenum err; bool wasError = false; while((err = glGetError()) != GL_NO_ERROR){wasError = true; std::cout << "[ERRRRRROOOOORRRRRRRRRR] OpenGL error in "  << __FILE__ << "(" << __LINE__ << ")  error code: " << err << std::endl;} assert(!wasError);}


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



