#include "flurr/renderer/VertexBuffer.h"
#include "flurr/FlurrLog.h"

namespace flurr
{

VertexBuffer::VertexBuffer(FlurrHandle a_bufferHandle)
  : m_bufferHandle(a_bufferHandle),
  m_bufferType(VertexBufferType::kVertexAttribute),
  m_dataSize(0),
  m_attributeSize(0),
  m_dataUsage(VertexDataUsage::kStatic),
  m_oglVboId(0)
{
}

Status VertexBuffer::initBuffer(VertexBufferType a_bufferType, std::size_t a_dataSize, void* a_data, std::size_t a_attributeSize, VertexDataUsage a_dataUsage)
{
  if (nullptr == a_data)
  {
    FLURR_LOG_ERROR("data cannot be null!");
    return Status::kNullArgument;
  }

  if (a_dataSize <= 0)
  {
    FLURR_LOG_ERROR("data size must be > 0!");
    return Status::kInvalidArgument;
  }

  if (a_attributeSize <= 0)
  {
    FLURR_LOG_ERROR("attribute size must be > 0!");
    return Status::kInvalidArgument;
  }

  if (m_oglVboId)
  {
    FLURR_LOG_ERROR("Vertex buffer already created!");
    return Status::kInvalidState;
  }

  // Set buffer properties
  m_bufferType = a_bufferType;
  m_dataUsage = a_dataUsage;
  m_dataSize = a_dataSize;
  m_attributeSize = a_attributeSize;

  // Determine OGL buffer type
  GLenum oglBufferType = getBufferType() == VertexBufferType::kVertexAttribute ?
    GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

  // Determine OGL buffer usage
  GLenum oglDataUsage = GL_STATIC_DRAW;
  switch (getDataUsage())
  {
  case VertexDataUsage::kStatic:
    oglDataUsage = GL_STATIC_DRAW;
    break;
  case VertexDataUsage::kDynamic:
    oglDataUsage = GL_DYNAMIC_DRAW;
    break;
  default:
    FLURR_ASSERT(false, "Unsupported OGL vertex buffer usage!");
    return Status::kFailed;
  }

  // Create OGL buffer and fill it with data
  glGenBuffers(1, &m_oglVboId);
  glBindBuffer(oglBufferType, m_oglVboId);
  glBufferData(oglBufferType, getDataSize(), a_data, oglDataUsage);

  return Status::kSuccess;
}

Status VertexBuffer::initIndexBuffer(std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage) {
  return initBuffer(VertexBufferType::kIndex, a_dataSize, a_data, sizeof(uint32_t), a_dataUsage);
}

void VertexBuffer::destroyBuffer()
{
  if (m_oglVboId)
    glDeleteBuffers(1, &m_oglVboId);

  m_dataSize = 0;
}

Status VertexBuffer::useBuffer()
{
  if (!isCreated())
  {
    FLURR_LOG_ERROR("Unable to use vertex buffer; not created yet!");
    return Status::kInvalidState;
  }

  GLenum oglBufferType = getBufferType() == VertexBufferType::kVertexAttribute ?
    GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
  glBindBuffer(oglBufferType, m_oglVboId);

  return Status::kSuccess;
}

} // namespace flurr
