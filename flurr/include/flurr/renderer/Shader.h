#pragma once

#include "flurr/FlurrDefines.h"

#include <string>

namespace flurr
{

class ShaderProgram;

enum class ShaderType : uint8_t
{
  kVertex = 0,
  kGeometry,
  kFragment
};

class FLURR_DLL_EXPORT Shader
{
  friend class ShaderProgram;

public:

  Shader(ShaderType a_shaderType, ShaderProgram* a_owningProgram);
  Shader(const Shader&) = delete;
  Shader(Shader&&) = default;
  Shader& operator=(const Shader&) = delete;
  Shader& operator=(Shader&&) = default;
  virtual ~Shader() = default;

  ShaderType getShaderType() const { return m_shaderType; }
  ShaderProgram* getOwningProgram() const { return m_owningProgram; }

private:

  virtual Status compile(FlurrHandle a_shaderResourceHandle) = 0;
  virtual void destroy() = 0;

  ShaderType m_shaderType;
  ShaderProgram* m_owningProgram;
};

} // namespace flurr
