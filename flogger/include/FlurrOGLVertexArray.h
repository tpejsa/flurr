#pragma once

#include "renderer/FlurrVertexArray.h"

#include <GL/glew.h>

namespace flurr {

class FLURR_DLL_EXPORT OGLVertexArray : public VertexArray {

public:

  OGLVertexArray(VertexArrayHandle array_handle);
  ~OGLVertexArray() override;

  inline GLuint GetOGLVertexArrayId() const { return ogl_vao_id_; }

private:

  VertexBuffer* OnCreateBuffer() override;
  void OnDeleteArray() override;
  Status OnDrawArray() override;

  GLuint ogl_vao_id_;
};

} // namespace flurr
