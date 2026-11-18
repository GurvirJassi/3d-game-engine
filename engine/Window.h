#pragma once
#include <array>
#include <string>

struct GLFWwindow;

namespace eng {
class Window {
 public:
  Window(int width, int height, const std::string& title, int msaaSamples = 4);
  ~Window();
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  bool shouldClose() const;
  void close();
  void pollEvents();  // call once per frame; updates input state
  void swapBuffers();
  void setTitle(const std::string& t);
  void setVSync(bool on);
  void setCursorCaptured(bool captured);

  bool cursorCaptured() const { return captured_; }
  bool keyDown(int key) const { return valid(key) && cur_[key]; }
  bool keyPressed(int key) const { return valid(key) && cur_[key] && !prev_[key]; }
  float mouseDX() const { return dx_; }
  float mouseDY() const { return dy_; }
  int fbWidth() const { return fbw_; }
  int fbHeight() const { return fbh_; }
  float aspect() const { return fbh_ > 0 ? float(fbw_) / float(fbh_) : 1.f; }

 private:
  static bool valid(int k) { return k >= 0 && k < 512; }
  GLFWwindow* win_ = nullptr;
  std::array<bool, 512> cur_{}, prev_{};
  bool captured_ = false, firstMouse_ = true;
  double lastX_ = 0, lastY_ = 0;
  float dx_ = 0, dy_ = 0;
  int fbw_ = 0, fbh_ = 0;
};
}  // namespace eng
