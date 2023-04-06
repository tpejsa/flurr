#include "flurr/renderer/ShaderProgram.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/TypeCasts.h"

#include <glm/gtc/type_ptr.hpp>

namespace flurr
{

ShaderProgram::ShaderProgram(FlurrHandle a_programHandle)
  : m_programHandle(a_programHandle),
  m_programState(ShaderProgramState::kDestroyed),
  m_oglProgramId(0)
{
}

bool ShaderProgram::setFloatValue(const std::string& a_name, float a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform1f(loc, a_value);
  return true;
}

bool ShaderProgram::setVec2Value(const std::string& a_name, const glm::vec2& a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform2f(loc, a_value.x, a_value.y);
  return true;
}

bool ShaderProgram::setVec3Value(const std::string& a_name, const glm::vec3& a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform3f(loc, a_value.x, a_value.y, a_value.z);
  return true;
}

bool ShaderProgram::setVec4Value(const std::string& a_name, const glm::vec4& a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform4f(loc, a_value.x, a_value.y, a_value.z, a_value.w);
  return true;
}

bool ShaderProgram::setMat4Value(const std::string& a_name, const glm::mat4& a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(a_value));
  return true;
}

bool ShaderProgram::setIntValue(const std::string& a_name, int a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform1i(loc, a_value);
  return true;
}

bool ShaderProgram::setUIntValue(const std::string& a_name, uint32_t a_value)
{
  GLint loc = glGetUniformLocation(m_oglProgramId, a_name.c_str());
  if (-1 == loc) return false;

  glUniform1ui(loc, a_value);
  return true;
}

bool ShaderProgram::setBoolValue(const std::string& a_name, bool a_value)
{
  return setIntValue(a_name, a_value);
}

Status ShaderProgram::compileShader(ShaderType a_shaderType, FlurrHandle a_shaderResourceHandle)
{
  if (ShaderProgramState::kLinked == m_programState)
  {
    FLURR_LOG_ERROR("Unable to compile shader, program already linked!");
    return Status::kInvalidState;
  }

  if (hasShader(a_shaderType))
  {
    FLURR_LOG_WARN("Compiling shader of type %u, which already exists in the current program!", FromEnum(a_shaderType));
    m_shadersByType.at(a_shaderType)->destroy();
  }

  m_shadersByType.emplace(a_shaderType, std::make_unique<Shader>(a_shaderType, this));
  auto* shader = getShader(a_shaderType);
  Status compileStatus = shader->compile(a_shaderResourceHandle);
  if (Status::kSuccess != compileStatus)
  {
    shader->destroy();
    m_shadersByType.erase(a_shaderType);
    return compileStatus;
  }

  m_programState = ShaderProgramState::kCompiled;

  return Status::kSuccess;
}

bool ShaderProgram::hasShader(ShaderType a_shaderType) const
{
  return m_shadersByType.find(a_shaderType) != m_shadersByType.end();
}

Shader* ShaderProgram::getShader(ShaderType a_shaderType) const
{
  auto&& shaderIt = m_shadersByType.find(a_shaderType);
  return m_shadersByType.end() == shaderIt ? nullptr : shaderIt->second.get();
}

Status ShaderProgram::linkProgram()
{
  if (ShaderProgramState::kCompiled != getProgramState())
  {
    FLURR_LOG_ERROR("Shader program not in compiled state!");
    return Status::kInvalidState;
  }

  m_oglProgramId = glCreateProgram();
  if (!m_oglProgramId)
  {
    FLURR_LOG_ERROR("Failed to create shader program!");
    return Status::kFailed;
  }

  // Link current program
  for (const auto& shaderKvp : m_shadersByType)
    glAttachShader(m_oglProgramId, shaderKvp.second.get()->getOGLShaderId());
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
  
  m_programState = ShaderProgramState::kLinked;

  // Safe to destroy shaders once program is linked
  for (auto& shaderKvp : m_shadersByType)
    shaderKvp.second.get()->destroy();

  return Status::kSuccess;
}

void ShaderProgram::destroyProgram()
{
  if (!m_oglProgramId)
    glDeleteProgram(m_oglProgramId);
  m_programState = ShaderProgramState::kDestroyed;

  // Also destroy shaders
  for (auto& shaderKvp : m_shadersByType)
    shaderKvp.second.get()->destroy();
  m_shadersByType.clear();
}

Status ShaderProgram::useProgram()
{
  if (ShaderProgramState::kLinked != getProgramState())
  {
    FLURR_LOG_ERROR("Shader program not in linked state!");
    return Status::kInvalidState;
  }

  glUseProgram(m_oglProgramId);
  return Status::kSuccess;
}

} // namespace flurr
