#pragma once

#include <vector>
#include <string>

#define setNames(namespaceName, ...) namespace namespaceName{ extern std::vector<std::string> filePaths; enum{ZERO_DUMMY, __VA_ARGS__  ,NAMECOUNT};}
#define setPaths(namespaceName, ...) namespace namespaceName {inline void setup() {filePaths = {"zero_dummy", __VA_ARGS__, "ending fag"}; filePaths.pop_back();}}

// here include your file \/

#include <TexNames.hpp>
#include <MeshNames.hpp>
#include <ShaderNames.hpp>

// ^


#undef setNames
#undef setPaths
