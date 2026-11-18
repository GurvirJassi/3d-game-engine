#pragma once
#include "GL.h"
#include <glm/glm.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace eng {
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

// Per-instance data streamed to the GPU (80 bytes).
struct InstanceData {
  glm::mat4 model;
  glm::vec4 color;
};
static_assert(sizeof(InstanceData) == 80, "InstanceData must be tightly packed");

class Mesh {
 public:
  Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
  ~Mesh();
  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  // Uploads instance data and draws everything in ONE draw call.
  void drawInstanced(const InstanceData* data, size_t count) const;
  float boundingRadius() const { return radius_; }

  static std::unique_ptr<Mesh> createCube();                       // 1x1x1, centred
  static std::unique_ptr<Mesh> createPlane();                      // 1x1 on XZ, centred
  static std::unique_ptr<Mesh> createSphere(int segments = 24, int rings = 16);  // diameter 1

 private:
  GLuint vao_ = 0, vbo_ = 0, ebo_ = 0, instVbo_ = 0;
  GLsizei indexCount_ = 0;
  float radius_ = 0.f;
};
}  // namespace eng
