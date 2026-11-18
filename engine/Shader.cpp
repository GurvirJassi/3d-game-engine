#include "Shader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace eng {
static std::string readFile(const std::string& path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) throw std::runtime_error("Cannot open shader: " + path);
  std::stringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

static GLuint compile(GLenum type, const std::string& src, const std::string& name) {
  GLuint s = glCreateShader(type);
  const char* p = src.c_str();
  glShaderSource(s, 1, &p, nullptr);
  glCompileShader(s);
  GLint ok = 0;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    GLint len = 0;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
    std::vector<char> log(len > 1 ? len : 1);
    glGetShaderInfoLog(s, len, nullptr, log.data());
    glDeleteShader(s);
    throw std::runtime_error("Shader compile error (" + name + "):\n" + log.data());
  }
  return s;
}

Shader::Shader(const std::string& vp, const std::string& fp) {
  GLuint vs = compile(GL_VERTEX_SHADER, readFile(vp), vp);
  GLuint fs = compile(GL_FRAGMENT_SHADER, readFile(fp), fp);
  id_ = glCreateProgram();
  glAttachShader(id_, vs);
  glAttachShader(id_, fs);
  glLinkProgram(id_);
  glDeleteShader(vs);
  glDeleteShader(fs);
  GLint ok = 0;
  glGetProgramiv(id_, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[2048];
    glGetProgramInfoLog(id_, sizeof(log), nullptr, log);
    throw std::runtime_error(std::string("Shader link error:\n") + log);
  }
}

Shader::~Shader() { if (id_) glDeleteProgram(id_); }

void Shader::bindUniformBlock(const char* name, GLuint binding) const {
  GLuint idx = glGetUniformBlockIndex(id_, name);
  if (idx != GL_INVALID_INDEX) glUniformBlockBinding(id_, idx, binding);
}
}  // namespace eng
