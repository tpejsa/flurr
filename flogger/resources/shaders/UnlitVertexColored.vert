#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

out vec3 color;

uniform mat4 worldTranfs;
uniform mat4 viewProjTransf;

void main()
{
  gl_Position = viewProjTransf*worldTranfs*vec4(inPos, 1.0f);
  color = inColor;
}
