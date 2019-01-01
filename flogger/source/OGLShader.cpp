#include "flogger/OGLShader.h"
#include "flurr/FlurrCore.h"
#include "flurr/resource/ShaderResource.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/TypeCasts.h"

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

Status OGLShader::compile(FlurrHandle a_shaderResourceHandle)
{
  GLenum oglShaderType = getOGLShaderType(getShaderType());
  if (0 == oglShaderType)
  {
    FLURR_LOG_ERROR("Unsupported shader type %u!", FromEnum(getShaderType()));
    return Status::kUnsupportedType;
  }

  // Get shader resource
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  auto* shaderResource = static_cast<ShaderResource*>(resourceManager->getResource(a_shaderResourceHandle));
  if (!shaderResource)
  {
    FLURR_LOG_ERROR("Unable to compile shader; shader resource %s does not exist!", shaderResource->getResourcePath().c_str());
    return Status::kInvalidHandle;
  }
  if (shaderResource->getResourceState() != ResourceState::kLoaded)
  {
    FLURR_LOG_ERROR("Unable to compile shader; shader resource %s not loaded!", shaderResource->getResourcePath().c_str());
    return Status::kResourceNotLoaded;
  }

  // Create and compile shader
  auto&& shaderResourceLock = shaderResource->lockResource();
  const auto& shaderSource = shaderResource->getShaderSource();
  const auto* shaderSourceData = shaderSource.data();
  GLint shaderSourceLength = static_cast<GLint>(shaderSource.length());
  m_oglShaderId = glCreateShader(oglShaderType);
  if (0 == m_oglShaderId)
  {
    FLURR_LOG_ERROR("Failed to create shader %s!", shaderResource->getResourcePath().c_str());
    return Status::kFailed;
  }
  glShaderSource(m_oglShaderId, 1, &shaderSourceData, &shaderSourceLength);
  glCompileShader(m_oglShaderId);
  shaderResourceLock.unlock();

  // Check for compilation errors
  GLint result = 0;
  glGetShaderiv(m_oglShaderId, GL_COMPILE_STATUS, &result);
  if (!result)
  {
    static const int kInfoLogSize = 1024;
    GLchar infoLog[kInfoLogSize];
    glGetShaderInfoLog(m_oglShaderId, kInfoLogSize, nullptr, &infoLog[0]);
    FLURR_LOG_ERROR("Failed to compile shader %s!\n%s", shaderResource->getResourcePath().c_str(), infoLog);

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
