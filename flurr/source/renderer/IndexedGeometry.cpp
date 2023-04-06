#include "flurr/renderer/IndexedGeometry.h"
#include "flurr/FlurrLog.h"
#include "flurr/FlurrCore.h"

namespace flurr
{

IndexedGeometry::IndexedGeometry(FlurrHandle a_geometryHandle)
  : m_geometryHandle(a_geometryHandle),
  m_geometryInitialized(false),
  m_indexBufferHandle(INVALID_HANDLE),
  m_oglVaoId(0)
{
}

VertexBuffer* IndexedGeometry::getAttributeBuffer(std::size_t a_attributeIndex) const {
  if (a_attributeIndex >= m_attributeBufferHandles.size())
  {
    FLURR_LOG_ERROR("No vertex attribute buffer at index %u!", static_cast<uint32_t>(a_attributeIndex));
    return nullptr;
  }

  auto* renderer = FlurrCore::Get().getRenderer();
  return renderer->getVertexBuffer(m_attributeBufferHandles[a_attributeIndex]);
}

VertexBuffer* IndexedGeometry::getIndexBuffer() const
{
  auto* renderer = FlurrCore::Get().getRenderer();
  return renderer->getVertexBuffer(m_indexBufferHandle);
}

Status IndexedGeometry::initGeometry(const std::vector<FlurrHandle>& a_attributeBufferHandles, FlurrHandle a_indexBufferHandle)
{
  if (m_geometryInitialized)
  {
    FLURR_LOG_ERROR("Unable to create indexed geometry; already created!");
    return Status::kInvalidState;
  }

  for (const auto& attributeBufferHandle : a_attributeBufferHandles)
  {
    auto result = addAttributeBuffer(attributeBufferHandle);
    if (result != Status::kSuccess)
      return result;
  }

  auto result = setIndexBuffer(a_indexBufferHandle);
  if (result != Status::kSuccess)
    return result;

  // Create OGL vertex array
  glGenVertexArrays(1, &m_oglVaoId);
  glBindVertexArray(m_oglVaoId);

  // Set OGL vertex array attribute pointers to our VBOs
  for (std::size_t attributeIndex = 0; attributeIndex < getAttributeBuffersCount(); ++attributeIndex)
  {
    const GLuint oglAttributeIndex = static_cast<GLuint>(attributeIndex);
    glBindBuffer(GL_ARRAY_BUFFER, getAttributeBuffer(attributeIndex)->getOGLVertexBufferObjectId());
    std::size_t attributeSize = getAttributeBuffer(attributeIndex)->getAttributeSize();
    glVertexAttribPointer(oglAttributeIndex,
      static_cast<GLint>(attributeSize) / sizeof(float), GL_FLOAT,
      GL_FALSE, 0, 0);
    glEnableVertexAttribArray(oglAttributeIndex);
  }

  m_geometryInitialized = true;
  return Status::kSuccess;
}

void IndexedGeometry::destroyGeometry()
{
  // Delete OGL vertex array
  if (m_oglVaoId)
    glDeleteVertexArrays(1, &m_oglVaoId);

  m_attributeBufferHandles.clear();
  m_indexBufferHandle = INVALID_HANDLE;
  m_geometryInitialized = false;
}

Status IndexedGeometry::drawGeometry()
{
  if (!m_geometryInitialized)
  {
    FLURR_LOG_ERROR("Unable to draw indexed geometry; not created yet!");
    return Status::kInvalidState;
  }

  // Get number of indices to draw
  auto* indexBuffer = getIndexBuffer();
  std::size_t numIndices = indexBuffer->getDataSize() / sizeof(uint32_t);

  // Draw the elements
  glBindVertexArray(m_oglVaoId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getOGLVertexBufferObjectId());
  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

  return Status::kSuccess;
}

Status IndexedGeometry::addAttributeBuffer(FlurrHandle a_bufferHandle)
{
  // Get pointer to the vertex buffer
  auto* renderer = FlurrCore::Get().getRenderer();
  auto* buffer = renderer->getVertexBuffer(a_bufferHandle);
  if (!buffer) {
    FLURR_LOG_ERROR("Unable to add vertex attribute buffer; no buffer with that handle!");
    return Status::kInvalidHandle;
  }

  if (buffer->getBufferType() != VertexBufferType::kVertexAttribute) {
    FLURR_LOG_ERROR("Unable to add vertex attribute buffer; wrong buffer type!");
    return Status::kUnsupportedType;
  }

  if (getAttributeBuffersCount() > 0 &&
    getAttributeBuffer(0)->getDataSize() / getAttributeBuffer(0)->getAttributeSize() != buffer->getDataSize() / buffer->getAttributeSize())
  {
    FLURR_LOG_ERROR("Buffer data count must match the data count of existing vertex attribute buffers!");
    return Status::kInvalidArgument;
  }

  // Add vertex attribute buffer to our vector
  m_attributeBufferHandles.push_back(a_bufferHandle);

  return Status::kSuccess;
}

Status IndexedGeometry::setIndexBuffer(FlurrHandle a_bufferHandle) {
  // Get pointer to the index buffer
  auto* renderer = FlurrCore::Get().getRenderer();
  auto* buffer = renderer->getVertexBuffer(a_bufferHandle);
  if (!buffer) {
    FLURR_LOG_ERROR("Unable to set index buffer; no buffer with that handle!");
    return Status::kInvalidHandle;
  }

  if (buffer->getBufferType() != VertexBufferType::kIndex) {
    FLURR_LOG_ERROR("Unable to set index buffer; wrong buffer type!");
    return Status::kUnsupportedType;
  }

  // TODO: should verify that indices are not out of bounds

  // Set it as our current index buffer
  m_indexBufferHandle = a_bufferHandle;

  return Status::kSuccess;
}

} // namespace flurr
