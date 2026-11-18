#include "Window.h"
#include "GL.h"
#include <stdexcept>

namespace eng {
Window::Window(int w, int h, const std::string& title, int msaa) {
  if (!glfwInit()) throw std::runtime_error("Failed to initialise GLFW");
  // OpenGL 3.3 core: the highest common denominator for Windows + macOS
  // (macOS will give us a 4.1 core context; forward-compat is mandatory there).
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, msaa);
#ifdef __APPLE__
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#endif
  win_ = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
  if (!win_) {
    glfwTerminate();
    throw std::runtime_error("Failed to create window / OpenGL 3.3 context");
  }
  glfwMakeContextCurrent(win_);
  glfwSetWindowUserPointer(win_, this);
  glfwSetKeyCallback(win_, [](GLFWwindow* wnd, int key, int, int action, int) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(wnd));
    if (self && valid(key)) self->cur_[key] = (action != GLFW_RELEASE);
  });

  glewExperimental = GL_TRUE;  // required for core profiles
  GLenum err = glewInit();
  glGetError();                // glewInit can leave a benign GL_INVALID_ENUM
  if (err != GLEW_OK) throw std::runtime_error("Failed to initialise GLEW");

  setVSync(true);
  glfwGetFramebufferSize(win_, &fbw_, &fbh_);
}

Window::~Window() {
  if (win_) glfwDestroyWindow(win_);
  glfwTerminate();
}

bool Window::shouldClose() const { return glfwWindowShouldClose(win_); }
void Window::close() { glfwSetWindowShouldClose(win_, GLFW_TRUE); }
void Window::swapBuffers() { glfwSwapBuffers(win_); }
void Window::setTitle(const std::string& t) { glfwSetWindowTitle(win_, t.c_str()); }
void Window::setVSync(bool on) { glfwSwapInterval(on ? 1 : 0); }

void Window::setCursorCaptured(bool c) {
  captured_ = c;
  glfwSetInputMode(win_, GLFW_CURSOR, c ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
  if (c && glfwRawMouseMotionSupported()) glfwSetInputMode(win_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  firstMouse_ = true;
}

void Window::pollEvents() {
  prev_ = cur_;
  glfwPollEvents();
  glfwGetFramebufferSize(win_, &fbw_, &fbh_);
  double x, y;
  glfwGetCursorPos(win_, &x, &y);
  if (firstMouse_) { lastX_ = x; lastY_ = y; firstMouse_ = false; }
  dx_ = float(x - lastX_);
  dy_ = float(y - lastY_);
  lastX_ = x; lastY_ = y;
}
}  // namespace eng
