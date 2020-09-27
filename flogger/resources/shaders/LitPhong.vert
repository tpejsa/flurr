#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;

out vec2 uv;

uniform mat4 modelMat;
uniform mat4 viewProjMat;

void main()
{
  gl_Position = viewProjMat*modelMat*vec4(inPos, 1.0f);
  uv = inUV;
}
