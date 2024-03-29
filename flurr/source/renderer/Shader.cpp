#include "flurr/renderer/Shader.h"
#include "flurr/renderer/ShaderProgram.h"
#include "flurr/resource/ShaderResource.h"
#include "flurr/FlurrCore.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/TypeCasts.h"

namespace flurr
{

Shader::Shader(ShaderType a_shaderType, ShaderProgram* a_owningProgram)
  : m_shaderType(a_shaderType),
  m_owningProgram(a_owningProgram),
  m_oglShaderId(0)
{
}

Status Shader::compile(FlurrHandle a_shaderResourceHandle)
{
  GLenum oglShaderType = getOGLShaderType(getShaderType());
  if (0 == oglShaderType)
  {
    FLURR_LOG_ERROR("Unsupported shader type %u!", FromEnum(getShaderType()));
    return Status::kUnsupportedType;
  }

  // Get shader resource
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  auto resourceLock = resourceManager->lockResources();
  auto* shaderResource = static_cast<ShaderResource*>(resourceManager->getResource(a_shaderResourceHandle));
  if (!shaderResource)
  {
    FLURR_LOG_ERROR("Unable to compile shader; shader resource %u does not exist!", a_shaderResourceHandle);
    return Status::kResourceNotCreated;
  }
  if (ResourceType::kShader != shaderResource->getResourceType())
  {
    FLURR_LOG_ERROR("Unable to compile shader; resource %s is not a shader resource!", shaderResource->getResourcePath().c_str());
    return Status::kResourceTypeInvalid;
  }
  if (shaderResource->getResourceState() != ResourceState::kLoaded)
  {
    FLURR_LOG_ERROR("Unable to compile shader; shader resource %s not loaded!", shaderResource->getResourcePath().c_str());
    return Status::kResourceNotLoaded;
  }

  // Create and compile shader
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
  resourceLock.unlock();

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

void Shader::destroy()
{
  if (m_oglShaderId)
    glDeleteShader(m_oglShaderId);
}

GLenum Shader::getOGLShaderType(ShaderType shader_type) const
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
