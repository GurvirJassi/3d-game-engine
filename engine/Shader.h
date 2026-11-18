#pragma once
#include "GL.h"
#include <string>

namespace eng {
class Shader {
 public:
  Shader(const std::string& vertPath, const std::string& fragPath);
  ~Shader();
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  void bind() const { glUseProgram(id_); }
  GLuint id() const { return id_; }
  void bindUniformBlock(const char* name, GLuint binding) const;

 private:
  GLuint id_ = 0;
};
}  // namespace eng
