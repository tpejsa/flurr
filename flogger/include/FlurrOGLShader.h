#pragma once

#include "renderer/FlurrShader.h"

#include <GL/glew.h>

namespace flurr {

class FLURR_DLL_EXPORT OGLShader : public Shader {

public:

  OGLShader(ShaderType shader_type, ShaderProgram* owning_program);
  ~OGLShader() override;

  Status Compile(const std::string& shader_path, ShaderCreateMode create_mode) override;
  void Delete() override;
  inline GLuint GetOGLShaderId() const { return ogl_shader_id_; }

private:

  GLenum GetOGLShaderType(ShaderType shader_type) const;

  GLuint ogl_shader_id_;
};

} // namespace flurr
