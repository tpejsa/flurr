#version 330 core

out vec4 outColor;

uniform vec3 albedo;

void main()
{
  outColor = vec4(albedo, 1.0f);
}
