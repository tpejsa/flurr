#pragma once

#include "flurr/renderer/VertexArray.h"

#include <GL/glew.h>

namespace flurr
{

class FLURR_DLL_EXPORT OGLVertexArray : public VertexArray
{

public:

  OGLVertexArray(FlurrHandle a_arrayHandle);
  ~OGLVertexArray() override;

  GLuint getOGLVertexArrayId() const { return m_oglVaoId; }

private:

  Status onCreateArray() override;
  void onDestroyArray() override;
  Status onDrawArray() override;

  GLuint m_oglVaoId;
};

} // namespace flurr
