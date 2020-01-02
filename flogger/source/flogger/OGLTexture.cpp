#include "flogger/OGLTexture.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/TypeCasts.h"

namespace flurr
{

OGLTexture::OGLTexture(FlurrHandle a_texHandle)
  : Texture(a_texHandle),
  m_oglTexId(0)
{
}

OGLTexture::~OGLTexture()
{
  if (m_oglTexId)
    glDeleteTextures(1, &m_oglTexId);
}

Status OGLTexture::onInitTexture()
{
  if (m_oglTexId)
  {
    FLURR_LOG_ERROR("Texture already created!");
    return Status::kInvalidState;
  }

  // Determine texture format
  GLint oglInternalTexFormat = GL_RGB8;
  GLint oglTexFormat = GL_RGB;
  auto* texResource = getTextureResource();
  switch (texResource->getTextureFormat())
  {
    case TextureFormat::kGrayscale:
    {
      oglInternalTexFormat = GL_R8;
      oglTexFormat = GL_RED;
      break;
    }
    case TextureFormat::kRGB:
    {
      oglInternalTexFormat = GL_RGB8;
      oglTexFormat = GL_RGB;
      break;
    }
    case TextureFormat::kRGBA:
    {
      oglInternalTexFormat = GL_RGBA8;
      oglTexFormat = GL_RGBA;
      break;
    }
    default:
    {
      FLURR_LOG_ERROR("Unsupported texture format %u for texture %u!",
        FromEnum(texResource->getTextureFormat()), getTextureHandle());
      return Status::kUnsupportedTextureFormat;
    }
  }

  // Create OGL texture ID
  glGenTextures(1, &m_oglTexId);

  // Determine texture wrap mode
  GLint oglTexWrapMode = GL_REPEAT;
  switch (getWrapMode())
  {
    case TextureWrapMode::kRepeat:
    {
      oglTexWrapMode = GL_REPEAT;
      break;
    }
    case TextureWrapMode::kMirroredRepeat:
    {
      oglTexWrapMode = GL_MIRRORED_REPEAT;
      break;
    }
    case TextureWrapMode::kClampToEdge:
    {
      oglTexWrapMode = GL_CLAMP_TO_EDGE;
      break;
    }
    case TextureWrapMode::kClampToBorder:
    {
      oglTexWrapMode = GL_CLAMP_TO_BORDER;
      break;
    }
    default:
    {
      break;
    }
  }

  // Determine texture minificiation filter mode
  GLint oglTexMinFilterMode = GL_LINEAR_MIPMAP_LINEAR;
  switch (getMinFilterMode())
  {
    case TextureMinFilterMode::kNearest:
    {
      oglTexMinFilterMode = GL_NEAREST;
      break;
    }
    case TextureMinFilterMode::kLinear:
    {
      oglTexMinFilterMode = GL_LINEAR;
      break;
    }
    case TextureMinFilterMode::kNearestMipmapNearest:
    {
      oglTexMinFilterMode = GL_NEAREST_MIPMAP_NEAREST;
      break;
    }
    case TextureMinFilterMode::kLinearMipmapNearest:
    {
      oglTexMinFilterMode = GL_LINEAR_MIPMAP_NEAREST;
      break;
    }
    case TextureMinFilterMode::kNearestMipmapLinear:
    {
      oglTexMinFilterMode = GL_NEAREST_MIPMAP_LINEAR;
      break;
    }
    case TextureMinFilterMode::kLinearMipmapLinear:
    {
      oglTexMinFilterMode = GL_LINEAR_MIPMAP_LINEAR;
      break;
    }
    default:
    {
      break;
    }
  }

  // Determine texture magnification filter mode
  GLint oglTexMagFilterMode = GL_LINEAR;
  switch (getMagFilterMode())
  {
    case TextureMagFilterMode::kNearest:
    {
      oglTexMagFilterMode = GL_NEAREST;
      break;
    }
    case TextureMagFilterMode::kLinear:
    {
      oglTexMagFilterMode = GL_LINEAR;
      break;
    }
    default:
    {
      break;
    }
  }

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

void OGLTexture::onDestroyTexture()
{
  if (m_oglTexId)
    glDeleteTextures(1, &m_oglTexId);
}

Status OGLTexture::onUseTexture(TextureUnitIndex a_texUnit)
{
  glActiveTexture(a_texUnit);
  glBindTexture(GL_TEXTURE_2D, m_oglTexId);
  return Status::kSuccess;
}

} // namespace flurr
