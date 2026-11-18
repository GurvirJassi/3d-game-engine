#include "Assets.h"
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;
namespace eng::assets {
static fs::path g_exeDir;

void init(const char* argv0) {
  std::error_code ec;
  if (argv0 && *argv0) g_exeDir = fs::absolute(fs::path(argv0), ec).parent_path();
}

std::string resolve(const std::string& rel) {
  std::error_code ec;
  const fs::path candidates[] = {
      g_exeDir / "assets" / rel,
      fs::path("assets") / rel,
      fs::path(ENGINE_SOURCE_ASSET_DIR) / rel,
  };
  for (const auto& c : candidates)
    if (!c.empty() && fs::exists(c, ec)) return c.string();
  throw std::runtime_error("Asset not found: " + rel);
}
}  // namespace eng::assets
