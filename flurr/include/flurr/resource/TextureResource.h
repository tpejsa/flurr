#pragma once

#include "flurr/resource/Resource.h"

namespace flurr
{

enum class TextureFormat : uint32_t
{
  kGrayscale,
  kRGB,
  kRGBA
};

class FLURR_DLL_EXPORT TextureResource : public Resource
{

  friend class ResourceManager;

protected:

  TextureResource(FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex, ResourceManager* a_owningManager);

public:

  ResourceType getResourceType() const override { return ResourceType::kTexture; }

  const uint8_t* getTextureData() const { return m_texData; }
  uint32_t getTextureWidth() const { return m_texWidth; }
  uint32_t getTextureHeight() const { return m_texHeight; }
  TextureFormat getTextureFormat() const { return m_texFormat; }
  uint32_t getTextureDataSize() const { return m_texWidth*m_texHeight*getTexelSize(); }
  uint32_t getTexelSize() const;

  static constexpr uint32_t TexelSize(const TextureFormat a_texFormat); // in bytes

private:

  Status onLoad(const std::string& a_fullPath) override;
  void onUnload() override;
  void onDestroy() override {}
  void destroyTextureData();
  void FlipY();

  uint8_t* m_texData;
  uint32_t m_texWidth;
  uint32_t m_texHeight;
  TextureFormat m_texFormat;
};

} // namespace flurr
