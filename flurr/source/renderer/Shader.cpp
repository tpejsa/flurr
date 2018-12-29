#include "flurr/renderer/Shader.h"
#include "flurr/renderer/ShaderProgram.h"

namespace flurr
{

Shader::Shader(ShaderType a_shaderType, ShaderProgram* a_owningProgram)
  : m_shaderType(a_shaderType),
  m_owningProgram(a_owningProgram)
{
}

} // namespace flurr
