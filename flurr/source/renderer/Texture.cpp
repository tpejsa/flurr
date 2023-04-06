#include "flurr/renderer/Texture.h"
#include "flurr/FlurrLog.h"
#include "flurr/FlurrCore.h"
#include "flurr/utils/TypeCasts.h"

namespace flurr
{

Texture::Texture(FlurrHandle a_texHandle)
  : m_texHandle(a_texHandle),
  m_texResourceHandle(INVALID_HANDLE),
  m_texWrapMode(TextureWrapMode::kRepeat),
  m_texMinFilterMode(TextureMinFilterMode::kLinearMipmapLinear),
  m_texMagFilterMode(TextureMagFilterMode::kLinear),
  m_oglTexId(0)
{
}

Status Texture::initTexture(FlurrHandle a_texResourceHandle, TextureWrapMode a_texWrapMode, TextureMinFilterMode a_texMinFilterMode, TextureMagFilterMode a_texMagFilterMode)
{
  if (m_oglTexId)
  {
    FLURR_LOG_ERROR("Texture already created!");
    return Status::kInvalidState;
  }

  // Ensure we have a valid and loaded texture resource
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  Resource* resource = nullptr;
  auto resourceLock = resourceManager->lockAndGetResource(&resource, a_texResourceHandle);
  if (!resource)
  {
    FLURR_LOG_ERROR("Unable to initialize texture; texture resource %u does not exist!", a_texResourceHandle);
    return Status::kResourceNotCreated;
  }
  if (ResourceType::kTexture != resource->getResourceType())
  {
    FLURR_LOG_ERROR("Unable to initialize texture; resource %s is not a texture resource!", resource->getResourcePath().c_str());
    return Status::kResourceTypeInvalid;
  }
  if (ResourceState::kLoaded != resource->getResourceState())
  {
    FLURR_LOG_ERROR("Unable to initialize texture; texture resource %s not loaded!", resource->getResourcePath().c_str());
    return Status::kResourceNotLoaded;
  }

  // Initialize texture
  m_texResourceHandle = a_texResourceHandle;
  m_texWrapMode = a_texWrapMode;
  m_texMinFilterMode = a_texMinFilterMode;
  m_texMagFilterMode = a_texMagFilterMode;
  auto* texResource = static_cast<TextureResource*>(resource);

  // Determine texture format
  GLint oglInternalTexFormat = GL_RGB8;
  GLint oglTexFormat = GL_RGB;
  if (!getOGLTextureFormat(texResource->getTextureFormat(), oglInternalTexFormat, oglTexFormat))
  {
    FLURR_LOG_ERROR("Unsupported texture format %u for texture %u!",
      FromEnum(texResource->getTextureFormat()), getTextureHandle());
    return Status::kUnsupportedTextureFormat;
  }

  // Create OGL texture ID
  glGenTextures(1, &m_oglTexId);

  // Determine texture wrap mode
  GLint oglTexWrapMode = getOGLTextureWrapMode(getWrapMode());

  // Determine texture minificiation filter mode
  GLint oglTexMinFilterMode = getOGLTextureMinFilterMode(getMinFilterMode());

  // Determine texture magnification filter mode
  GLint oglTexMagFilterMode = getOGLTextureMagFilterMode(getMagFilterMode());

  // Set texture wrap and filter parameters
  glBindTexture(GL_TEXTURE_2D, m_oglTexId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, oglTexWrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, oglTexWrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, oglTexMinFilterMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, oglTexMagFilterMode);

  // Load texture data
  glTexImage2D(GL_TEXTURE_2D, 0, oglInternalTexFormat, texResource->getTextureWidth(), texResource->getTextureHeight(),
    0, oglTexFormat, GL_UNSIGNED_BYTE, texResource->getTextureData());
  if (getMinFilterMode() >= TextureMinFilterMode::kNearestMipmapNearest &&
    getMinFilterMode() <= TextureMinFilterMode::kLinearMipmapLinear)
  {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  return Status::kSuccess;
}

void Texture::destroyTexture()
{
  if (m_oglTexId)
    glDeleteTextures(1, &m_oglTexId);

  m_texResourceHandle = INVALID_HANDLE;
}

Status Texture::useTexture(TextureUnitIndex a_texUnit)
{
  if (INVALID_HANDLE == m_texResourceHandle)
  {
    FLURR_LOG_ERROR("Unable to use texture; not created yet!");
    return Status::kInvalidState;
  }

  if (a_texUnit > MAX_TEXTURE_UNIT)
  {
    FLURR_LOG_ERROR("Texture unit index out of bounds!");
    return Status::kIndexOutOfBounds;
  }

  glActiveTexture(GL_TEXTURE0 + a_texUnit);
  glBindTexture(GL_TEXTURE_2D, m_oglTexId);

  return Status::kSuccess;
}

bool Texture::getOGLTextureFormat(TextureFormat a_texFormat, GLint& a_oglInternalTexFormat, GLint& a_oglTexFormat) const
{
  switch (a_texFormat)
  {
  case TextureFormat::kGrayscale:
    a_oglInternalTexFormat = GL_R8;
    a_oglTexFormat = GL_RED;
    break;
  case TextureFormat::kRGB:
    a_oglInternalTexFormat = GL_RGB8;
    a_oglTexFormat = GL_RGB;
    break;
  case TextureFormat::kRGBA:
    a_oglInternalTexFormat = GL_RGBA8;
    a_oglTexFormat = GL_RGBA;
    break;
  default:
    return false;
  }

  return true;
}

GLint Texture::getOGLTextureWrapMode(TextureWrapMode a_texWrapMode) const
{
  switch (a_texWrapMode)
  {
  case TextureWrapMode::kRepeat:
    return GL_REPEAT;
  case TextureWrapMode::kMirroredRepeat:
    return GL_MIRRORED_REPEAT;
  case TextureWrapMode::kClampToEdge:
    return GL_CLAMP_TO_EDGE;
  case TextureWrapMode::kClampToBorder:
    return GL_CLAMP_TO_BORDER;
  default:
    return GL_REPEAT;
  }
}


GLint Texture::getOGLTextureMinFilterMode(TextureMinFilterMode a_texMinFilterMode) const
{
  switch (a_texMinFilterMode)
  {
  case TextureMinFilterMode::kNearest:
    return GL_NEAREST;
  case TextureMinFilterMode::kLinear:
    return GL_LINEAR;
  case TextureMinFilterMode::kNearestMipmapNearest:
    return GL_NEAREST_MIPMAP_NEAREST;
  case TextureMinFilterMode::kLinearMipmapNearest:
    return GL_LINEAR_MIPMAP_NEAREST;
  case TextureMinFilterMode::kNearestMipmapLinear:
    return GL_NEAREST_MIPMAP_LINEAR;
  case TextureMinFilterMode::kLinearMipmapLinear:
    return GL_LINEAR_MIPMAP_LINEAR;
  default:
    return GL_LINEAR_MIPMAP_LINEAR;
  }
}


GLint Texture::getOGLTextureMagFilterMode(TextureMagFilterMode a_texMagFilterMode) const
{
  switch (a_texMagFilterMode)
  {
  case TextureMagFilterMode::kNearest:
    return GL_NEAREST;
  case TextureMagFilterMode::kLinear:
    return GL_LINEAR;
  default:
    return GL_LINEAR;
  }
}

} // namespace flurr
