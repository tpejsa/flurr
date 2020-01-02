#version 330 core

out vec4 outColor;

in vec2 uv;

uniform vec4 diffuseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform bool diffuseMapEnabled = true;
uniform sampler2D diffuseMap;

void main()
{
  outColor = vec4(1.0f);
  if (diffuseMapEnabled)
    outColor = texture(diffuseMap, uv);
  outColor *= diffuseColor;
}
