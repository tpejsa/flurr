#pragma once

#include "flurr/FlurrDefines.h"

#include <GL/glew.h>

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
  ~Shader() = default;

  ShaderType getShaderType() const { return m_shaderType; }
  ShaderProgram* getOwningProgram() const { return m_owningProgram; }

  GLuint getOGLShaderId() const { return m_oglShaderId; }

private:

  Status compile(FlurrHandle a_shaderResourceHandle);
  void destroy();

  GLenum getOGLShaderType(ShaderType a_shaderType) const;

  ShaderType m_shaderType;
  ShaderProgram* m_owningProgram;
  GLuint m_oglShaderId;
};

} // namespace flurr
