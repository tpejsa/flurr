#include "flogger/OGLShaderProgram.h"
#include "flogger/OGLShader.h"
#include "flurr/FlurrLog.h"

#include <string>

namespace flurr
{

OGLShaderProgram::OGLShaderProgram(FlurrHandle a_programHandle)
  : ShaderProgram(a_programHandle),
  m_oglProgramId(0)
{
}

OGLShaderProgram::~OGLShaderProgram()
{
  if (!m_oglProgramId)
    glDeleteProgram(m_oglProgramId);
}

Shader* OGLShaderProgram::onCreateShader(ShaderType a_shaderType, ShaderProgram* a_owningProgram)
{
  return new OGLShader(a_shaderType, a_owningProgram);
}

Status OGLShaderProgram::onLinkProgram()
{
  m_oglProgramId = glCreateProgram();
  if (!m_oglProgramId)
  {
    FLURR_LOG_ERROR("Failed to create shader program!");
    return Status::kFailed;
  }

  // Link current program
  const auto& shaders = getAllShaders();
  for (auto* shader : shaders)
    glAttachShader(m_oglProgramId, static_cast<OGLShader*>(shader)->getOGLShaderId());
  glLinkProgram(m_oglProgramId);

  // Check for linking errors
  GLint result;
  glGetProgramiv(m_oglProgramId, GL_LINK_STATUS, &result);
  if (!result)
  {
    static const int kInfoLogSize = 1024;
    GLchar infoLog[kInfoLogSize];
    glGetProgramInfoLog(m_oglProgramId, kInfoLogSize, nullptr, &infoLog[0]);
    FLURR_LOG_ERROR("Failed to link shader program!\n%s", infoLog);

    return Status::kLinkingFailed;
  }

  return Status::kSuccess;
}

void OGLShaderProgram::onDestroyProgram()
{
  if (!m_oglProgramId)
    glDeleteProgram(m_oglProgramId);
}

void OGLShaderProgram::onUseProgram()
{
  glUseProgram(m_oglProgramId);
}

} // namespace flurr
