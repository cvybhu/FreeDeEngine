#include <Utils.hpp>
#include <GLFW/glfw3.h>

#include <iomanip>
#include <iostream>
#include <fstream>

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

