#include "flogger/OGLShaderProgram.h"
#include "flogger/OGLShader.h"
#include "flurr/FlurrLog.h"

#include <glm/gtc/type_ptr.hpp>

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

bool OGLShaderProgram::setFloatValue(const std::string& a_name, float a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform1f(loc, a_value);
  return true;
}

bool OGLShaderProgram::setVec2Value(const std::string& a_name, const glm::vec2& a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform2f(loc, a_value.x, a_value.y);
  return true;
}

bool OGLShaderProgram::setVec3Value(const std::string& a_name, const glm::vec3& a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform3f(loc, a_value.x, a_value.y, a_value.z);
  return true;
}

bool OGLShaderProgram::setVec4Value(const std::string& a_name, const glm::vec4& a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform4f(loc, a_value.x, a_value.y, a_value.z, a_value.w);
  return true;
}

bool OGLShaderProgram::setMat4Value(const std::string& a_name, const glm::mat4& a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(a_value));
  return true;
}

bool OGLShaderProgram::setIntValue(const std::string& a_name, int a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform1i(loc, a_value);
  return true;
}

bool OGLShaderProgram::setUIntValue(const std::string& a_name, uint32_t a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform1ui(loc, a_value);
  return true;
}

bool OGLShaderProgram::setBoolValue(const std::string& a_name, bool a_value)
{
  return setIntValue(a_name, a_value);
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
