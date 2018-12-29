#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

out vec3 color;

void main()
{
  color = inColor;
  gl_Position = vec4(inPos, 1.0f);
}
