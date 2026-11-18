#include "Renderer.h"
#include "Assets.h"
#include <algorithm>
#include <cmath>

namespace eng {
Renderer::Renderer() {
  shader_ = std::make_unique<Shader>(assets::resolve("shaders/basic.vert"),
                                     assets::resolve("shaders/basic.frag"));
  shader_->bindUniformBlock("Frame", 0);
  glGenBuffers(1, &ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(FrameUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);   // skip back faces
  glCullFace(GL_BACK);
  glEnable(GL_MULTISAMPLE);
}

Renderer::~Renderer() { glDeleteBuffers(1, &ubo_); }

void Renderer::beginFrame(const Camera& cam, int fbw, int fbh) {
  stats_ = {};
  glViewport(0, 0, fbw, fbh);
  // Shader outputs gamma-corrected colour, so convert the clear colour too.
  glm::vec3 clear = glm::pow(fogColor, glm::vec3(1.f / 2.2f));
  glClearColor(clear.r, clear.g, clear.b, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float aspect = fbh > 0 ? float(fbw) / float(fbh) : 1.f;
  glm::mat4 vp = cam.projection(aspect) * cam.view();

  FrameUBO u;
  u.viewProj = vp;
  u.camPos = glm::vec4(cam.position, 1.f);
  u.lightDir = glm::vec4(glm::normalize(lightDirection), 0.f);
  u.lightColor = glm::vec4(lightColor, 1.f);
  u.ambient = glm::vec4(ambient, 1.f);
  u.fog = glm::vec4(fogColor, fogDensity);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FrameUBO), &u);

  // Gribb/Hartmann frustum plane extraction (column-major glm matrices)
  auto row = [&](int i) { return glm::vec4(vp[0][i], vp[1][i], vp[2][i], vp[3][i]); };
  planes_[0] = row(3) + row(0);
  planes_[1] = row(3) - row(0);
  planes_[2] = row(3) + row(1);
  planes_[3] = row(3) - row(1);
  planes_[4] = row(3) + row(2);
  planes_[5] = row(3) - row(2);
  for (auto& p : planes_) p /= glm::length(glm::vec3(p));
}

void Renderer::submit(const Mesh& mesh, const glm::mat4& model, const glm::vec4& color) {
  ++stats_.submitted;
  glm::vec3 c(model[3]);
  float s = std::max({glm::length(glm::vec3(model[0])), glm::length(glm::vec3(model[1])),
                      glm::length(glm::vec3(model[2]))});
  float r = mesh.boundingRadius() * s;
  for (const auto& p : planes_)
    if (glm::dot(glm::vec3(p), c) + p.w < -r) return;  // fully outside this plane
  batches_[&mesh].push_back({model, color});
}

void Renderer::endFrame() {
  shader_->bind();
  for (auto& [mesh, list] : batches_) {
    if (list.empty()) continue;
    mesh->drawInstanced(list.data(), list.size());
    stats_.drawn += uint32_t(list.size());
    ++stats_.drawCalls;
    list.clear();  // keeps capacity -> no per-frame allocations
  }
}
}  // namespace eng
