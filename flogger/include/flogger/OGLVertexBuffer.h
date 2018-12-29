#pragma once

#include "flurr/renderer/VertexBuffer.h"

#include <GL/glew.h>

namespace flurr
{

class FLURR_DLL_EXPORT OGLVertexBuffer : public VertexBuffer
{

public:

  OGLVertexBuffer(FlurrHandle a_bufferHandle);
  ~OGLVertexBuffer() override;

  GLuint getOGLVertexBufferId() const { return m_oglVboId; }

private:

  Status onCreateBuffer() override;
  void onDestroyBuffer() override;
  Status onUseBuffer() override;

  GLuint m_oglVboId;
};

} // namespace flurr
