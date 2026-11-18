#include "Mesh.h"
#include <cmath>
#include <cstddef>

namespace eng {
Mesh::Mesh(const std::vector<Vertex>& v, const std::vector<uint32_t>& idx) {
  indexCount_ = GLsizei(idx.size());
  for (const auto& vert : v) radius_ = std::max(radius_, glm::length(vert.position));

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
  glGenBuffers(1, &instVbo_);
  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(Vertex), v.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(uint32_t), idx.data(), GL_STATIC_DRAW);

  // Instance attributes: mat4 = locations 2..5, colour = 6
  glBindBuffer(GL_ARRAY_BUFFER, instVbo_);
  for (int i = 0; i < 4; ++i) {
    glEnableVertexAttribArray(2 + i);
    glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                          (void*)(offsetof(InstanceData, model) + sizeof(glm::vec4) * i));
    glVertexAttribDivisor(2 + i, 1);
  }
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
  glVertexAttribDivisor(6, 1);

  glBindVertexArray(0);
}

Mesh::~Mesh() {
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteBuffers(1, &instVbo_);
  glDeleteVertexArrays(1, &vao_);
}

void Mesh::drawInstanced(const InstanceData* data, size_t count) const {
  if (!count) return;
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, instVbo_);
  // Re-specifying the store each frame "orphans" the old one and avoids GPU stalls.
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(count * sizeof(InstanceData)), data, GL_STREAM_DRAW);
  glDrawElementsInstanced(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr, GLsizei(count));
}

std::unique_ptr<Mesh> Mesh::createCube() {
  const glm::vec3 normals[6] = {{0, 0, 1}, {0, 0, -1}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}};
  std::vector<Vertex> v;
  std::vector<uint32_t> idx;
  for (const auto& n : normals) {
    glm::vec3 tmp = std::fabs(n.y) > 0.5f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
    glm::vec3 u = glm::normalize(glm::cross(tmp, n));
    glm::vec3 w = glm::cross(n, u);  // cross(u,w) == n  -> CCW seen from outside
    glm::vec3 c = n * 0.5f;
    uint32_t base = uint32_t(v.size());
    v.push_back({c - u * 0.5f - w * 0.5f, n});
    v.push_back({c + u * 0.5f - w * 0.5f, n});
    v.push_back({c + u * 0.5f + w * 0.5f, n});
    v.push_back({c - u * 0.5f + w * 0.5f, n});
    idx.insert(idx.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
  }
  return std::make_unique<Mesh>(v, idx);
}

std::unique_ptr<Mesh> Mesh::createPlane() {
  glm::vec3 n(0, 1, 0);
  std::vector<Vertex> v = {{{-.5f, 0, .5f}, n}, {{.5f, 0, .5f}, n}, {{.5f, 0, -.5f}, n}, {{-.5f, 0, -.5f}, n}};
  std::vector<uint32_t> idx = {0, 1, 2, 0, 2, 3};
  return std::make_unique<Mesh>(v, idx);
}

std::unique_ptr<Mesh> Mesh::createSphere(int segs, int rings) {
  std::vector<Vertex> v;
  std::vector<uint32_t> idx;
  const float pi = 3.14159265358979f;
  for (int r = 0; r <= rings; ++r) {
    float theta = pi * float(r) / float(rings);
    for (int s = 0; s <= segs; ++s) {
      float phi = 2.f * pi * float(s) / float(segs);
      glm::vec3 n(std::sin(theta) * std::cos(phi), std::cos(theta), std::sin(theta) * std::sin(phi));
      v.push_back({n * 0.5f, n});
    }
  }
  for (int r = 0; r < rings; ++r)
    for (int s = 0; s < segs; ++s) {
      uint32_t a = uint32_t(r * (segs + 1) + s), b = a + uint32_t(segs + 1);
      idx.insert(idx.end(), {a, a + 1, b, a + 1, b + 1, b});
    }
  return std::make_unique<Mesh>(v, idx);
}
}  // namespace eng
