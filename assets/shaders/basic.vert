#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in mat4 aModel;   // occupies locations 2-5
layout(location = 6) in vec4 aColor;

layout(std140) uniform Frame {
  mat4 uViewProj;
  vec4 uCamPos;
  vec4 uLightDir;
  vec4 uLightColor;
  vec4 uAmbient;
  vec4 uFog;
};

out vec3 vWorldPos;
out vec3 vNormal;
out vec4 vColor;

void main() {
  vec4 world = aModel * vec4(aPos, 1.0);
  vWorldPos = world.xyz;
  vNormal = mat3(aModel) * aNormal;   // correct for uniform scale; use inverse-transpose for non-uniform
  vColor = aColor;
  gl_Position = uViewProj * world;
}
