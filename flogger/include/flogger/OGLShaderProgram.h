#pragma once

#include "flurr/renderer/ShaderProgram.h"
#include "flogger/OGLRenderer.h"

#include <GL/glew.h>

namespace flurr
{

class FLURR_DLL_EXPORT OGLShaderProgram : public ShaderProgram
{

public:

  OGLShaderProgram(FlurrHandle a_programHandle);
  ~OGLShaderProgram() override;

  GLuint getOGLProgramId() const { return m_oglProgramId; }

private:

  Shader* onCreateShader(ShaderType a_shaderType, ShaderProgram* a_owningProgram) override;
  Status onLinkProgram() override;
  void onDestroyProgram() override;
  void onUseProgram() override;

  GLuint m_oglProgramId;
};

} // namespace flurr
