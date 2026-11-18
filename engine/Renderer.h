#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace eng {
class Renderer {
 public:
  struct Stats {
    uint32_t submitted = 0;  // objects submitted this frame
    uint32_t drawn = 0;      // objects that survived frustum culling
    uint32_t drawCalls = 0;  // GL draw calls issued
  };

  // Scene environment (tweak freely)
  glm::vec3 lightDirection{-0.4f, -1.0f, -0.3f};
  glm::vec3 lightColor{1.0f, 0.95f, 0.85f};
  glm::vec3 ambient{0.35f, 0.42f, 0.55f};
  glm::vec3 fogColor{0.55f, 0.65f, 0.78f};  // also the sky/clear colour (linear)
  float fogDensity = 0.006f;

  Renderer();
  ~Renderer();
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  void beginFrame(const Camera& camera, int fbWidth, int fbHeight);
  // Frustum-culled, then batched per mesh so each mesh costs one instanced draw call.
  void submit(const Mesh& mesh, const glm::mat4& model, const glm::vec4& color);
  void endFrame();

  const Stats& stats() const { return stats_; }

 private:
  struct FrameUBO {
    glm::mat4 viewProj;
    glm::vec4 camPos;
    glm::vec4 lightDir;
    glm::vec4 lightColor;
    glm::vec4 ambient;
    glm::vec4 fog;  // rgb = colour, a = density
  };
  std::unique_ptr<Shader> shader_;
  GLuint ubo_ = 0;
  glm::vec4 planes_[6];
  std::unordered_map<const Mesh*, std::vector<InstanceData>> batches_;
  Stats stats_;
};
}  // namespace eng
