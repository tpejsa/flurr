#include "FlurrOGLVertexArray.h"
#include "FlurrOGLVertexBuffer.h"
#include "FlurrLog.h"

namespace flurr {

OGLVertexArray::OGLVertexArray(VertexArrayHandle array_handle)
  : VertexArray(array_handle),
  ogl_vao_id_(0) {
}

OGLVertexArray::~OGLVertexArray() {
  if (ogl_vao_id_)
    glDeleteVertexArrays(1, &ogl_vao_id_);
}

VertexBuffer* OGLVertexArray::OnCreateBuffer() {
  return new OGLVertexBuffer();
}

void OGLVertexArray::OnDeleteArray() {

}

Status OGLVertexArray::OnDrawArray() {

}

} // namespace flurr
