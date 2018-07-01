#pragma once

#include "renderer/FlurrShaderProgram.h"
#include "FlurrOGLRenderer.h"

#include <GL/glew.h>

namespace flurr {

class FLURR_DLL_EXPORT OGLShaderProgram : public ShaderProgram {

public:

  OGLShaderProgram(ShaderProgramHandle program_handle);
  ~OGLShaderProgram() override;

  inline GLuint GetOGLProgramId() const { return ogl_program_id_; }

private:

  Shader* OnCreateShader(ShaderType shader_type, ShaderProgram* owning_program) override;
  Status OnLinkProgram() override;
  void OnDeleteProgram() override;

  GLuint ogl_program_id_;
};

} // namespace flurr
