#pragma once
#include "Window.h"
#include <glm/glm.hpp>

namespace eng {
// Free-fly first-person camera.
class Camera {
 public:
  glm::vec3 position{0.f, 5.f, 10.f};
  float yaw = -90.f, pitch = -10.f;   // degrees
  float fov = 70.f, nearZ = 0.1f, farZ = 600.f;
  float moveSpeed = 12.f, fastMultiplier = 4.f, mouseSensitivity = 0.1f;

  void update(const Window& window, float dt);
  glm::vec3 forward() const;
  glm::mat4 view() const;
  glm::mat4 projection(float aspect) const;
};
}  // namespace eng
