# MiniEngine

A small, readable 3D engine in C++17 + OpenGL 3.3 core. Runs on **Windows** and **macOS** (also Linux).
The demo renders ~12,000 objects at high frame rates using only a handful of draw calls.

## Build

Requires CMake 3.20+, a C++17 compiler, and git. Dependencies (GLFW, GLM, GLEW) are downloaded automatically.

**macOS** (Xcode command line tools):
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/demo
```

**Windows** (Visual Studio 2019+ with C++ workload):
```
cmake -S . -B build
cmake --build build --config Release
build\Release\demo.exe
```

## Controls
WASD move · Q/E down/up · Shift sprint · Mouse look · Tab free/capture cursor · V toggle vsync · Esc quit

## Layout
```
engine/      Window, Shader, Mesh, Camera, Transform, Renderer, Assets
assets/      GLSL shaders (copied next to the executable on build)
demo/        main.cpp - sample scene
```

## Performance features
- **GPU instancing**: one draw call per mesh, per-instance matrix + colour streamed in one buffer.
- **CPU frustum culling**: sphere-vs-plane tests before anything reaches the GPU.
- **Uniform buffer object** for per-frame data (camera, light, fog), uploaded once per frame.
- **Back-face culling, depth testing, 4x MSAA**, buffer orphaning (no GPU stalls), no per-frame allocations.
- Static objects have precomputed matrices; link-time optimisation enabled in Release.
- Cheap lighting: Blinn-Phong, hemispheric ambient, exponential fog, gamma correction in-shader.

## Notes
- macOS caps OpenGL at 4.1 and deprecates it; 3.3 core is stable and fast there. Moving to Metal/Vulkan
  would mean replacing `Mesh`/`Shader`/`Renderer` behind the same interface.
- The vertex shader uses `mat3(model)` for normals, which assumes uniform scale (the demo's towers use
  non-uniform scale, so their lighting is slightly approximate; swap in an inverse-transpose if it matters).

## Ideas for next steps
Shadow maps, textures + mipmaps, glTF loading, an ECS, spatial partitioning (BVH/octree), LODs, audio, physics.
