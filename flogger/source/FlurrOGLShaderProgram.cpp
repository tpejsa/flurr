#include "FlurrOGLShaderProgram.h"
#include "FlurrOGLShader.h"
#include "FlurrLog.h"

#include <string>

namespace flurr {

OGLShaderProgram::OGLShaderProgram(ShaderProgramHandle program_handle)
  : ShaderProgram(program_handle),
  ogl_program_id_(0) {
}

OGLShaderProgram::~OGLShaderProgram() {
  if (!ogl_program_id_)
    glDeleteProgram(ogl_program_id_);
}

Shader* OGLShaderProgram::OnCreateShader(ShaderType shader_type, ShaderProgram* owning_program) {
  return new OGLShader(shader_type, owning_program);
}

Status OGLShaderProgram::OnLinkProgram() {
  ogl_program_id_ = glCreateProgram();
  if (!ogl_program_id_) {
    FLURR_LOG_ERROR("Failed to create shader program!");
    return Status::kFailed;
  }

  // Link current program
  const auto& shaders = GetAllShaders();
  for (auto* shader : shaders)
    glAttachShader(ogl_program_id_, static_cast<OGLShader*>(shader)->GetOGLShaderId());
  glLinkProgram(ogl_program_id_);

  // Check for linking errors
  GLint result;
  glGetProgramiv(ogl_program_id_, GL_LINK_STATUS, &result);
  if (!result) {
    static const int kInfoLogSize = 1024;
    GLchar info_log[kInfoLogSize];
    glGetProgramInfoLog(ogl_program_id_, kInfoLogSize, nullptr, &info_log[0]);
    FLURR_LOG_ERROR("Failed to link shader program!\n%s", info_log);

    return Status::kLinkingFailed;
  }
}

void OGLShaderProgram::OnDeleteProgram() {
  if (!ogl_program_id_)
    glDeleteProgram(ogl_program_id_);

  const auto& shaders = GetAllShaders();
  for (auto* shader : shaders)
    shader->Delete();
}

void OGLShaderProgram::OnUseProgram() {
  glUseProgram(ogl_program_id_);
}

} // namespace flurr
