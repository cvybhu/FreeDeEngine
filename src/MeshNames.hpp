#pragma once

#include <vector>
#include <string>

namespace meshNames{extern std::vector<std::string> filePaths; enum{ZERO_DUMMY = __COUNTER__};}

#define addMesh(...) addResource(meshNames, __VA_ARGS__)


addMesh(plane, "mesh/plane.obj");





#undef addTex
