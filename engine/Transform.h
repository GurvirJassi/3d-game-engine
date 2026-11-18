#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace eng {
struct Transform {
  glm::vec3 position{0.f};
  glm::quat rotation{1.f, 0.f, 0.f, 0.f};
  glm::vec3 scale{1.f};

  glm::mat4 matrix() const {
    return glm::translate(glm::mat4(1.f), position) * glm::mat4_cast(rotation) *
           glm::scale(glm::mat4(1.f), scale);
  }
};
}  // namespace eng
