#include "flogger/OGLVertexBuffer.h"
#include "flurr/FlurrLog.h"

namespace flurr
{

OGLVertexBuffer::OGLVertexBuffer(FlurrHandle a_bufferHandle)
  : VertexBuffer(a_bufferHandle),
  m_oglVboId(0)
{
}

OGLVertexBuffer::~OGLVertexBuffer()
{
  if (m_oglVboId)
    glDeleteBuffers(1, &m_oglVboId);
}

Status OGLVertexBuffer::onInitBuffer()
{
  if (m_oglVboId)
  {
    FLURR_LOG_ERROR("Vertex buffer already created!");
    return Status::kInvalidState;
  }

  // Determine OGL buffer type
  GLenum oglBufferType = getBufferType() == VertexBufferType::kVertexAttribute ?
    GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

  // Determine OGL buffer usage
  GLenum oglDataUsage = GL_STATIC_DRAW;
  switch (getDataUsage())
  {
    case VertexDataUsage::kStatic:
    {
      oglDataUsage = GL_STATIC_DRAW;
      break;
    }
    case VertexDataUsage::kDynamic:
    {
      oglDataUsage = GL_DYNAMIC_DRAW;
      break;
    }
    default:
    {
      FLURR_ASSERT(false, "Unsupported OGL vertex buffer usage!");
      return Status::kFailed;
    }
  }

  // Create OGL buffer and fill it with data
  glGenBuffers(1, &m_oglVboId);
  glBindBuffer(oglBufferType, m_oglVboId);
  glBufferData(oglBufferType, getDataSize(), getData(), oglDataUsage);

  return Status::kSuccess;
}

void OGLVertexBuffer::onDestroyBuffer()
{
  if (m_oglVboId)
    glDeleteBuffers(1, &m_oglVboId);
}

Status OGLVertexBuffer::onUseBuffer()
{
  GLenum oglBufferType = getBufferType() == VertexBufferType::kVertexAttribute ?
    GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
  glBindBuffer(oglBufferType, m_oglVboId);

  return Status::kSuccess;
}

} // namespace flurr
