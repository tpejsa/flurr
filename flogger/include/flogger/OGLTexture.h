#pragma once

#include "flurr/renderer/Texture.h"

#include <GL/glew.h>

namespace flurr
{

class FLURR_DLL_EXPORT OGLTexture : public Texture
{

public:

  OGLTexture(FlurrHandle a_bufferHandle);
  ~OGLTexture() override;

  GLuint getOGLTextureId() const { return m_oglTexId; }

private:

  Status onInitTexture() override;
  void onDestroyTexture() override;
  Status onUseTexture(TextureUnitIndex a_texUnit) override;

  GLuint m_oglTexId;
};

} // namespace flurr
