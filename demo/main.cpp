#include "Assets.h"
#include "Camera.h"
#include "GL.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Transform.h"
#include "Window.h"

#include <cstdio>
#include <random>
#include <stdexcept>
#include <vector>

struct StaticObject {
  const eng::Mesh* mesh;
  glm::mat4 model;  // precomputed once - static objects cost nothing per frame
  glm::vec4 color;
};
struct Spinner {
  const eng::Mesh* mesh;
  glm::vec3 position, axis;
  float speed, size;
  glm::vec4 color;
};

int main(int argc, char** argv) {
  try {
    eng::assets::init(argc > 0 ? argv[0] : "");
    eng::Window window(1280, 720, "MiniEngine", 4);
    window.setCursorCaptured(true);
    eng::Renderer renderer;
    eng::Camera camera;
    camera.position = {0.f, 12.f, 40.f};

    auto cube = eng::Mesh::createCube();
    auto sphere = eng::Mesh::createSphere();
    auto plane = eng::Mesh::createPlane();

    std::mt19937 rng(1234);
    std::uniform_real_distribution<float> u01(0.f, 1.f);

    std::vector<StaticObject> statics;
    statics.push_back({plane.get(),
                       eng::Transform{{0, 0, 0}, {1, 0, 0, 0}, {600, 1, 600}}.matrix(),
                       {0.25f, 0.30f, 0.22f, 1.f}});
    // 100 x 100 city of towers = 10,000 cubes
    for (int x = -50; x < 50; ++x)
      for (int z = -50; z < 50; ++z) {
        float h = 0.5f + u01(rng) * u01(rng) * 12.f;
        float shade = 0.3f + 0.5f * u01(rng);
        statics.push_back({cube.get(),
                           eng::Transform{{x * 3.f, h * 0.5f, z * 3.f - 20.f}, {1, 0, 0, 0}, {1.6f, h, 1.6f}}.matrix(),
                           {shade, shade * 0.9f, shade * 0.8f, 1.f}});
      }
    std::vector<Spinner> spinners;
    for (int i = 0; i < 2000; ++i)
      spinners.push_back({u01(rng) > 0.5f ? sphere.get() : cube.get(),
                          {(u01(rng) - .5f) * 280.f, 15.f + u01(rng) * 25.f, (u01(rng) - .5f) * 280.f - 20.f},
                          glm::normalize(glm::vec3(u01(rng) - .5f, u01(rng) + .1f, u01(rng) - .5f)),
                          0.5f + u01(rng) * 2.f, 0.8f + u01(rng) * 1.5f,
                          {0.3f + u01(rng) * .7f, 0.3f + u01(rng) * .7f, 0.3f + u01(rng) * .7f, 1.f}});

    bool vsync = true;
    double last = glfwGetTime(), fpsTimer = 0;
    int frames = 0;

    while (!window.shouldClose()) {
      double now = glfwGetTime();
      float dt = float(std::min(now - last, 0.1));  // clamp long stalls
      last = now;

      window.pollEvents();
      if (window.keyPressed(GLFW_KEY_ESCAPE)) window.close();
      if (window.keyPressed(GLFW_KEY_TAB)) window.setCursorCaptured(!window.cursorCaptured());
      if (window.keyPressed(GLFW_KEY_V)) { vsync = !vsync; window.setVSync(vsync); }
      camera.update(window, dt);

      renderer.beginFrame(camera, window.fbWidth(), window.fbHeight());
      for (const auto& o : statics) renderer.submit(*o.mesh, o.model, o.color);
      for (const auto& s : spinners) {
        eng::Transform t;
        t.position = s.position;
        t.rotation = glm::angleAxis(float(now) * s.speed, s.axis);
        t.scale = glm::vec3(s.size);
        renderer.submit(*s.mesh, t.matrix(), s.color);
      }
      renderer.endFrame();
      window.swapBuffers();

      ++frames;
      fpsTimer += dt;
      if (fpsTimer >= 0.5) {
        const auto& st = renderer.stats();
        char title[160];
        std::snprintf(title, sizeof title, "MiniEngine | %.0f FPS | %u/%u drawn | %u draw calls | vsync %s",
                      frames / fpsTimer, st.drawn, st.submitted, st.drawCalls, vsync ? "on" : "off");
        window.setTitle(title);
        fpsTimer = 0;
        frames = 0;
      }
    }
  } catch (const std::exception& e) {
    std::fprintf(stderr, "Fatal: %s\n", e.what());
    return 1;
  }
  return 0;
}
