#pragma once

#include "flurr/resource/Resource.h"

namespace flurr
{

enum class TextureFormat : uint32_t
{
  kGrayscale,
  kBGR,
  kBGRA
};

class FLURR_DLL_EXPORT TextureResource : public Resource
{

  friend class ResourceManager;

protected:

  TextureResource(FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex, ResourceManager* a_owningManager);

public:

  ResourceType getResourceType() const override { return ResourceType::kTexture; }

  const uint8_t* getTextureData() const { return m_textureData; }
  uint32_t getTextureWidth() const { return m_textureWidth; }
  uint32_t getTextureHeight() const { return m_textureHeight; }
  TextureFormat getTextureFormat() const { return m_textureFormat; }

private:

  Status onLoad(const std::string& a_fullPath) override;
  void onUnload() override;
  void onDestroy() override {}
  void destroyTextureData();

  uint8_t* m_textureData;
  uint32_t m_textureWidth;
  uint32_t m_textureHeight;
  TextureFormat m_textureFormat;
};

} // namespace flurr
