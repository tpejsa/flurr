#include "flurr/resource/ShaderResource.h"
#include "flurr/FlurrLog.h"

#include <fstream>
#include <streambuf>

namespace flurr
{

ShaderResource::ShaderResource(FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex, ResourceManager* a_owningManager)
  : Resource(a_resourceHandle, a_resourcePath, a_resourceDirectoryIndex, a_owningManager)
{
}

Status ShaderResource::onLoad(const std::string& a_fullPath)
{
  // Load shader source
  std::ifstream ifs(a_fullPath);
  if (!ifs.good())
  {
    FLURR_LOG_ERROR("Failed to read shader source %s!", a_fullPath.c_str());
    return Status::kOpenFileError;
  }
  m_shaderSource = std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  // TODO: add support for precompiled shaders

  return Status::kSuccess;
}

void ShaderResource::onUnload()
{
  // Clear shader source
  m_shaderSource.clear();
}

} // namespace flurr
