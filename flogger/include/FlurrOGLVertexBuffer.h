#pragma once

#include "renderer/FlurrVertexBuffer.h"

#include <GL/glew.h>

namespace flurr {

class FLURR_DLL_EXPORT OGLVertexBuffer : public VertexBuffer {

public:

  OGLVertexBuffer();
  ~OGLVertexBuffer() override;

  inline GLuint GetOGLVertexBufferId() const { return ogl_vbo_id_; }

private:

  Status OnCreateBuffer() override;
  Status OnDeleteBuffer() override;

  GLuint ogl_vbo_id_;
};

} // namespace flurr
