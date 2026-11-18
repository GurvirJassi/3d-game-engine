cmake_minimum_required(VERSION 3.20)
project(MiniEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Lets older third-party CMake files (GLEW) configure under CMake 4.x
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT HAS_IPO OUTPUT IPO_MSG)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4 GIT_SHALLOW TRUE)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1 GIT_SHALLOW TRUE)

set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glew
  URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
  SOURCE_SUBDIR build/cmake)

FetchContent_MakeAvailable(glfw glm glew)

# ---- Engine library ----
add_library(engine STATIC
  engine/Assets.cpp
  engine/Window.cpp
  engine/Shader.cpp
  engine/Mesh.cpp
  engine/Camera.cpp
  engine/Renderer.cpp)
target_include_directories(engine PUBLIC engine ${glew_SOURCE_DIR}/include)
target_compile_definitions(engine PUBLIC GLEW_STATIC)
target_compile_definitions(engine PRIVATE
  ENGINE_SOURCE_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_link_libraries(engine PUBLIC glfw glm::glm glew_s)
if(MSVC)
  target_compile_options(engine PUBLIC /W3 /utf-8)
else()
  target_compile_options(engine PUBLIC -Wall -Wextra)
endif()
if(HAS_IPO)
  set_property(TARGET engine PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

# ---- Demo ----
add_executable(demo demo/main.cpp)
target_link_libraries(demo PRIVATE engine)
if(HAS_IPO)
  set_property(TARGET demo PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()
add_custom_command(TARGET demo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:demo>/assets)
if(MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT demo)
endif()
