#include "Camera.h"
#include "GL.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace eng {
glm::vec3 Camera::forward() const {
  float y = glm::radians(yaw), p = glm::radians(pitch);
  return glm::normalize(glm::vec3(std::cos(p) * std::cos(y), std::sin(p), std::cos(p) * std::sin(y)));
}

void Camera::update(const Window& w, float dt) {
  if (w.cursorCaptured()) {
    yaw += w.mouseDX() * mouseSensitivity;
    pitch = std::clamp(pitch - w.mouseDY() * mouseSensitivity, -89.f, 89.f);
  }
  glm::vec3 f = forward();
  glm::vec3 r = glm::normalize(glm::cross(f, glm::vec3(0, 1, 0)));
  float s = moveSpeed * dt * (w.keyDown(GLFW_KEY_LEFT_SHIFT) ? fastMultiplier : 1.f);
  if (w.keyDown(GLFW_KEY_W)) position += f * s;
  if (w.keyDown(GLFW_KEY_S)) position -= f * s;
  if (w.keyDown(GLFW_KEY_D)) position += r * s;
  if (w.keyDown(GLFW_KEY_A)) position -= r * s;
  if (w.keyDown(GLFW_KEY_E)) position.y += s;
  if (w.keyDown(GLFW_KEY_Q)) position.y -= s;
}

glm::mat4 Camera::view() const { return glm::lookAt(position, position + forward(), glm::vec3(0, 1, 0)); }
glm::mat4 Camera::projection(float aspect) const {
  return glm::perspective(glm::radians(fov), aspect, nearZ, farZ);
}
}  // namespace eng
