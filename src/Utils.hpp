#pragma once
#include <string>

class TimeTeller
{
public:
    TimeTeller(const char* Name, double TellCooldown);

    void startMeasuring();
    void stopMeasuring();

    void tell();
private:
    const char* name;
    double startTime;
    const double tellCooldown;
    double lastTellTime;
    double maxTime;
};

std::string readFile(const char* path);



