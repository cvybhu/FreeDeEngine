#include <TimeTeller.hpp>
#include <Logger.hpp>
#include <iomanip>

TimeTeller::TimeTeller(const char* Name, double TellCooldown) : tellCooldown(TellCooldown)
{
    maxTime = -1;
    name = Name;
    lastTellTime = cock.now();
}

void TimeTeller::startMeasuring()
{
    startTime = cock.now();
}

double TimeTeller::getTimeFrom(time_point t)
{
    return std::chrono::duration<double>(cock.now() - t).count();
}


void TimeTeller::stopMeasuring()
{
    double curTime = getTimeFrom(startTime);

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
    if(getTimeFrom(lastTellTime) >= tellCooldown)
    {
        std::cout << name << std::setprecision(5) << std::fixed << ": " << maxTime*1000.0 << "ms\n";
        lastTellTime = cock.now();
        maxTime = -1;
    }
}

