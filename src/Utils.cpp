#include <Utils.hpp>
#include <GLFW/glfw3.h>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

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
