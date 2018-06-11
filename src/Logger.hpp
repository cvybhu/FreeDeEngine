#pragma once

#include <iostream>


class Log
{
public:
    //template <class T>
    //static std::ostream& operator<<(std::ostream& strim, const T& a){return std::cout << a;}
};

#include <glm/vec3.hpp>
inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v){return os << "(" << v.x << "," << v.y << "," << v.z << ")", os;}