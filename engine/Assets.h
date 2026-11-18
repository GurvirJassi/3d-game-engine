#pragma once
#include <string>

namespace eng::assets {
// Call once at startup with argv[0] so assets next to the executable are found.
void init(const char* argv0);
// Resolves "shaders/basic.vert" to a real path (exe dir, cwd, then source tree).
std::string resolve(const std::string& relative);
}
