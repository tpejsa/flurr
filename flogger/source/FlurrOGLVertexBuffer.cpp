#include "FlurrOGLVertexBuffer.h"
#include "FlurrLog.h"

namespace flurr {

OGLVertexBuffer::OGLVertexBuffer()
  : ogl_vbo_id_(0) {
}

OGLVertexBuffer::~OGLVertexBuffer() {
  if (ogl_vbo_id_)
    glDeleteBuffers(1, &ogl_vbo_id_);
}

Status OGLVertexBuffer::OnCreateBuffer() {
  if (ogl_vbo_id_) {
    FLURR_LOG_ERROR("Vertex buffer already created!");
    return Status::kInvalidState;
  }

  // Determine OGL buffer type
  GLenum ogl_buffer_type = GetBufferType() == VertexBufferType::kVertexAttribute ?
    GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

  // Determine OGL buffer usage
  GLenum ogl_data_usage = GL_STATIC_DRAW;
  switch (GetDataUsage()) {
    case VertexDataUsage::kStatic: {
      ogl_data_usage = GL_STATIC_DRAW;
      break;
    }
    case VertexDataUsage::kDynamic: {
      ogl_data_usage = GL_DYNAMIC_DRAW;
      break;
    }
    default: {
      FLURR_ASSERT(false, "Unsupported OGL vertex buffer usage!");
      return Status::kFailed;
    }
  }

  // Create OGL buffer
  glGenBuffers(1, &ogl_vbo_id_);
  glBindBuffer(ogl_buffer_type, ogl_vbo_id_);
  glBufferData(ogl_buffer_type, GetDataSize(), GetData(), ogl_data_usage);

  return Status::kSuccess;
}

Status OGLVertexBuffer::OnDeleteBuffer() {
  if (ogl_vbo_id_)
    glDeleteBuffers(1, &ogl_vbo_id_);
}

} // namespace flurr
