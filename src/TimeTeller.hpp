#pragma once

#include <chrono>


class TimeTeller    //TimeTeller is used to measure time multiple times and tell max measured time from every <cooldown> seconds
{                   //tell() only "tells" max time when cooldown wears off
public:
    TimeTeller(const char* Name, double TellCooldown);

    void startMeasuring();
    void stopMeasuring();

    void tell();
private:
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

    double getTimeFrom(time_point t);

    const char* name;
    time_point startTime;

    const double tellCooldown;
    time_point lastTellTime;

    double maxTime;

    std::chrono::high_resolution_clock cock;
};
