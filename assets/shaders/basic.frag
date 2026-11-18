#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec4 vColor;
out vec4 FragColor;

layout(std140) uniform Frame {
  mat4 uViewProj;
  vec4 uCamPos;
  vec4 uLightDir;
  vec4 uLightColor;
  vec4 uAmbient;
  vec4 uFog;
};

void main() {
  vec3 N = normalize(vNormal);
  vec3 L = normalize(-uLightDir.xyz);
  vec3 V = normalize(uCamPos.xyz - vWorldPos);
  vec3 H = normalize(L + V);

  float ndl = max(dot(N, L), 0.0);
  vec3 ambient = uAmbient.rgb * mix(0.4, 1.0, N.y * 0.5 + 0.5);   // cheap hemispheric ambient
  vec3 diffuse = uLightColor.rgb * ndl;
  float spec = pow(max(dot(N, H), 0.0), 64.0) * ndl * 0.35;

  vec3 col = vColor.rgb * (ambient + diffuse) + uLightColor.rgb * spec;

  float d = length(uCamPos.xyz - vWorldPos);
  float fog = 1.0 - exp(-pow(d * uFog.a, 2.0));
  col = mix(col, uFog.rgb, clamp(fog, 0.0, 1.0));

  FragColor = vec4(pow(col, vec3(1.0 / 2.2)), vColor.a);   // linear -> gamma
}
