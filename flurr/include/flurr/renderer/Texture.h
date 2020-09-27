#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/resource/TextureResource.h"

#include <glm/glm.hpp>

#include <mutex>

namespace flurr
{

enum class TextureWrapMode {
  kRepeat = 0,
  kMirroredRepeat,
  kClampToEdge,
  kClampToBorder
};

enum class TextureMinFilterMode {
  kNearest = 0,
  kLinear,
  kNearestMipmapNearest,
  kLinearMipmapNearest,
  kNearestMipmapLinear,
  kLinearMipmapLinear
};

enum class TextureMagFilterMode {
  kNearest = 0,
  kLinear
};

using TextureUnitIndex = uint32_t;
constexpr TextureUnitIndex MAX_TEXTURE_UNIT = 15;

class FLURR_DLL_EXPORT Texture
{
  friend class Renderer;

public:

  Texture(FlurrHandle a_texHandle);
  Texture(const Texture&) = default;
  Texture(Texture&&) = default;
  Texture& operator=(const Texture&) = default;
  Texture& operator=(Texture&&) = default;
  virtual ~Texture() = default;

  FlurrHandle getTextureHandle() const { return m_texHandle; }
  FlurrHandle getResourceHandle() const { return m_texResourceHandle; }  
  TextureWrapMode getWrapMode() { return m_texWrapMode; }
  TextureMinFilterMode getMinFilterMode() const { return m_texMinFilterMode; }
  TextureMagFilterMode getMagFilterMode() const { return m_texMagFilterMode; }

protected:

  TextureResource* getTextureResource() const;

private:

  Status initTexture(FlurrHandle a_texResourceHandle, TextureWrapMode a_texWrapMode = TextureWrapMode::kRepeat, TextureMinFilterMode a_texMinFilterMode = TextureMinFilterMode::kLinearMipmapLinear, TextureMagFilterMode a_texMagFilterMode = TextureMagFilterMode::kLinear);
  void destroyTexture();
  Status useTexture(TextureUnitIndex a_texUnit = 0);
  virtual Status onInitTexture() = 0;
  virtual void onDestroyTexture() = 0;
  virtual Status onUseTexture(TextureUnitIndex a_texUnit) = 0;

  FlurrHandle m_texHandle;
  FlurrHandle m_texResourceHandle;
  TextureWrapMode m_texWrapMode;
  TextureMinFilterMode m_texMinFilterMode;
  TextureMagFilterMode m_texMagFilterMode;
};

} // namespace flurr
