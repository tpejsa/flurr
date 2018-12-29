#include "flogger/OGLShader.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/TypeCasts.h"

#include <fstream>
#include <streambuf>
#include <string>

namespace flurr
{

OGLShader::OGLShader(ShaderType a_shaderType, ShaderProgram* a_owningProgram)
  : Shader(a_shaderType, a_owningProgram),
  m_oglShaderId(0)
{
}

OGLShader::~OGLShader()
{
  if (m_oglShaderId)
    glDeleteShader(m_oglShaderId);
}

Status OGLShader::compile(const std::string& a_shaderPath, ShaderCreateMode a_createMode)
{
  GLenum oglShaderType = getOGLShaderType(getShaderType());
  if (0 == oglShaderType)
  {
    FLURR_LOG_ERROR("Unsupported shader type %u!", FromEnum(getShaderType()));
    return Status::kUnsupportedType;
  }

  if (ShaderCreateMode::kFromSource != a_createMode)
  {
    FLURR_LOG_ERROR("Shader can only be compiled from source!");
    return Status::kUnsupportedMode;
  }

  // Load shader source
  std::ifstream ifs(a_shaderPath);
  if (!ifs.good())
  {
    FLURR_LOG_ERROR("Failed to read shader source %s!", a_shaderPath.c_str());
    return Status::kOpenFileError;
  }
  std::string shaderSourceStr((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  const auto* shaderSourceData = shaderSourceStr.data();
  GLint shaderSourceLength = static_cast<GLint>(shaderSourceStr.length());

  // Create and compile shader
  m_oglShaderId = glCreateShader(oglShaderType);
  if (0 == m_oglShaderId)
  {
    FLURR_LOG_ERROR("Failed to create shader!");
    return Status::kFailed;
  }
  glShaderSource(m_oglShaderId, 1, &shaderSourceData, &shaderSourceLength);
  glCompileShader(m_oglShaderId);

  // Check for compilation errors
  GLint result = 0;
  glGetShaderiv(m_oglShaderId, GL_COMPILE_STATUS, &result);
  if (!result)
  {
    static const int kInfoLogSize = 1024;
    GLchar infoLog[kInfoLogSize];
    glGetShaderInfoLog(m_oglShaderId, kInfoLogSize, nullptr, &infoLog[0]);
    FLURR_LOG_ERROR("Failed to compile shader %s!\n%s", a_shaderPath.c_str(), infoLog);

    return Status::kCompilationFailed;
  }

  return Status::kSuccess;
}

void OGLShader::destroy()
{
  if (m_oglShaderId)
    glDeleteShader(m_oglShaderId);
}

GLenum OGLShader::getOGLShaderType(ShaderType shader_type) const
{
  switch (shader_type)
  {
    case ShaderType::kVertex:
    {
      return GL_VERTEX_SHADER;
    }
    case ShaderType::kGeometry:
    {
      return GL_GEOMETRY_SHADER;
    }
    case ShaderType::kFragment:
    {
      return GL_FRAGMENT_SHADER;
    }
    default:
    {
      return 0;
    }
  }
}

} // namespace flurr
