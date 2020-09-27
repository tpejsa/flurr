#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;

out vec2 uv;

uniform mat4 modelTransf;
uniform mat4 viewProjTransf;

void main()
{
  gl_Position = viewProjTransf * modelTransf * vec4(inPos, 1.0f);
  uv = inUV;
}
