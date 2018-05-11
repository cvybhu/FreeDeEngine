#include <TimeTeller.hpp>
#include <GLFW/glfw3.h>

#include <Logger.hpp>

#include <iomanip>

TimeTeller::TimeTeller(std::string Name, double TellCooldown) : tellCooldown(TellCooldown)
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
        Log << name << std::setprecision(5) << std::fixed << ": " << maxTime*1000.0 << "ms\n";
        lastTellTime = glfwGetTime();
        maxTime = -1;
    }
}
