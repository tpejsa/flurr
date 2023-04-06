#version 330 core

in vec3 color;
out vec4 outColor;

uniform vec3 colorTint = vec3(1.0f, 1.0f, 1.0f);

void main()
{
  outColor = vec4(color * colorTint, 1.0f);
}
