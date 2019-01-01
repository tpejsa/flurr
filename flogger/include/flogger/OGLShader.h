#pragma once

#include "flurr/renderer/Shader.h"

#include <GL/glew.h>

namespace flurr
{

class FLURR_DLL_EXPORT OGLShader : public Shader
{

public:

  OGLShader(ShaderType a_shaderType, ShaderProgram* a_owningProgram);
  ~OGLShader() override;

  Status compile(FlurrHandle a_shaderResourceHandle) override;
  void destroy() override;
  GLuint getOGLShaderId() const { return m_oglShaderId; }

private:

  GLenum getOGLShaderType(ShaderType a_shaderType) const;

  GLuint m_oglShaderId;
};

} // namespace flurr
