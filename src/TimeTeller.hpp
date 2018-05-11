#pragma once
#include <string>


class TimeTeller
{
public:
    TimeTeller(std::string Name, double TellCooldown);

    void startMeasuring();
    void stopMeasuring();

    void tell();
private:
    std::string name;
    double startTime;
    const double tellCooldown;
    double lastTellTime;
    double maxTime;
};
