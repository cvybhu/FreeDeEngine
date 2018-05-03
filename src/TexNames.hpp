#pragma once

#include <vector>
#include <string>

namespace texNames{extern std::vector<std::string> filePaths; enum{ZERO_DUMMY = __COUNTER__};}

#define addTex(...) addResource(texNames, __VA_ARGS__)





addTex(container, "tex/container.jpg")
addTex(SHREK, "tex/god.png")
addTex(SHREKS_DICK, "tex/gods_giant_double_tower.tiff")
addTex(SHREKS_BALLS, "tex/gods_enligthening_roly_poly.bmp")





#undef addTex
