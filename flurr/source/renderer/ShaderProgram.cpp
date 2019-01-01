#include "flurr/renderer/ShaderProgram.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/TypeCasts.h"

namespace flurr
{

ShaderProgram::ShaderProgram(FlurrHandle a_programHandle)
  : m_programHandle(a_programHandle),
  m_programState(ShaderProgramState::kDestroyed)
{
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

  auto* shader = onCreateShader(a_shaderType, this);
  Status compileStatus = shader->compile(a_shaderResourceHandle);
  if (Status::kSuccess != compileStatus)
  {
    shader->destroy();
    delete shader;
    return compileStatus;
  }
  m_shadersByType.emplace(a_shaderType, std::unique_ptr<Shader>(shader));
  m_shaders.push_back(shader);

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

  Status linkStatus = onLinkProgram();
  if (Status::kSuccess == linkStatus)
  {
    m_programState = ShaderProgramState::kLinked;

    // Safe to destroy shaders once program is linked
    const auto& shaders = getAllShaders();
    for (auto* shader : shaders)
      shader->destroy();
  }
  else
  {
    destroyProgram();
  }

  return linkStatus;
}

void ShaderProgram::destroyProgram()
{
  onDestroyProgram();
  m_programState = ShaderProgramState::kDestroyed;

  // Also destroy shaders
  const auto& shaders = getAllShaders();
  for (auto* shader : shaders)
    shader->destroy();
  m_shadersByType.clear();
  m_shaders.clear();
}

Status ShaderProgram::useProgram()
{
  if (ShaderProgramState::kLinked != getProgramState())
  {
    FLURR_LOG_ERROR("Shader program not in linked state!");
    return Status::kInvalidState;
  }

  onUseProgram();
  return Status::kSuccess;
}

} // namespace flurr
