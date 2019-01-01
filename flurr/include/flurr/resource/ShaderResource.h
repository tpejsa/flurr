#pragma once

#include "flurr/resource/Resource.h"

namespace flurr
{

class FLURR_DLL_EXPORT ShaderResource : public Resource
{

  friend class ResourceManager;

protected:

  ShaderResource(FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex, ResourceManager* a_owningManager);

public:

  ResourceType getResourceType() const override { return ResourceType::kShader; }
  const std::string& getShaderSource() const { return m_shaderSource; }

private:

  Status onLoad(const std::string& a_fullPath) override;
  void onUnload() override;
  void onDestroy() override {}

  std::string m_shaderSource;
};

} // namespace flurr
