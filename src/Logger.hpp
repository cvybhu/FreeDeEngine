#pragma once

#include <iostream>

class Logger //lol TODO in the future
{
public:
    template <class T>
    std::ostream& operator<<(const T& t){return std::cout << t;}
};


extern Logger Log;
