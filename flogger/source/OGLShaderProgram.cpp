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

bool OGLShaderProgram::setFloatValue(const std::string& name, float value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, name.c_str());
  if (-1 == loc) return false;

  glUniform1f(loc, value);
  return true;
}

bool OGLShaderProgram::setVec2Value(const std::string& name, const glm::vec2& value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, name.c_str());
  if (-1 == loc) return false;

  glUniform2f(loc, value.x, value.y);
  return true;
}

bool OGLShaderProgram::setVec3Value(const std::string& name, const glm::vec3& value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, name.c_str());
  if (-1 == loc) return false;

  glUniform3f(loc, value.x, value.y, value.z);
  return true;
}

bool OGLShaderProgram::setVec4Value(const std::string& name, const glm::vec4& value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, name.c_str());
  if (-1 == loc) return false;

  glUniform4f(loc, value.x, value.y, value.z, value.w);
  return true;
}

bool OGLShaderProgram::setIntValue(const std::string& name, int value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, name.c_str());
  if (-1 == loc) return false;

  glUniform1i(loc, value);
  return true;
}

bool OGLShaderProgram::setUIntValue(const std::string& name, uint32_t value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, name.c_str());
  if (-1 == loc) return false;

  glUniform1ui(loc, value);
  return true;
}

bool OGLShaderProgram::setBoolValue(const std::string& name, bool value)
{
  return setIntValue(name, value);
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
