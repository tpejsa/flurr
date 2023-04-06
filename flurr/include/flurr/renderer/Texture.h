#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/resource/TextureResource.h"

#include <glm/glm.hpp>
#include <GL/glew.h>

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
  Texture(const Texture&) = delete;
  Texture(Texture&&) = default;
  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&&) = default;
  ~Texture() = default;

  FlurrHandle getTextureHandle() const { return m_texHandle; }
  FlurrHandle getResourceHandle() const { return m_texResourceHandle; }  
  TextureWrapMode getWrapMode() { return m_texWrapMode; }
  TextureMinFilterMode getMinFilterMode() const { return m_texMinFilterMode; }
  TextureMagFilterMode getMagFilterMode() const { return m_texMagFilterMode; }

private:

  Status initTexture(FlurrHandle a_texResourceHandle, TextureWrapMode a_texWrapMode = TextureWrapMode::kRepeat, TextureMinFilterMode a_texMinFilterMode = TextureMinFilterMode::kLinearMipmapLinear, TextureMagFilterMode a_texMagFilterMode = TextureMagFilterMode::kLinear);
  void destroyTexture();
  Status useTexture(TextureUnitIndex a_texUnit = 0);
  bool getOGLTextureFormat(TextureFormat a_texFormat, GLint& a_oglInternalTexFormat, GLint& a_oglTexFormat) const;
  GLint getOGLTextureWrapMode(TextureWrapMode a_texWrapMode) const;
  GLint getOGLTextureMinFilterMode(TextureMinFilterMode a_texMinFilterMode) const;
  GLint getOGLTextureMagFilterMode(TextureMagFilterMode a_texMagFilterMode) const;

  FlurrHandle m_texHandle;
  FlurrHandle m_texResourceHandle;
  TextureWrapMode m_texWrapMode;
  TextureMinFilterMode m_texMinFilterMode;
  TextureMagFilterMode m_texMagFilterMode;

  GLuint m_oglTexId;
};

} // namespace flurr
