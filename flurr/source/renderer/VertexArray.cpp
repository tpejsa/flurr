#include "flurr/renderer/VertexArray.h"
#include "flurr/FlurrLog.h"
#include "flurr/FlurrCore.h"

namespace flurr
{

VertexArray::VertexArray(FlurrHandle a_arrayHandle)
  : m_arrayHandle(a_arrayHandle),
  m_isArrayInitialized(false),
  m_indexBufferHandle(INVALID_HANDLE)
{
}

Status VertexArray::addAttributeBuffer(FlurrHandle a_bufferHandle)
{
  if (m_isArrayInitialized)
  {
    FLURR_LOG_ERROR("Unable to add vertex attribute buffer; vertex array already created!");
    return Status::kInvalidState;
  }

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
    getAttributeBuffer(0)->getDataSize()/getAttributeBuffer(0)->getAttributeSize() != buffer->getDataSize()/buffer->getAttributeSize())
  {
    FLURR_LOG_ERROR("Buffer data count must match the data count of existing vertex attribute buffers!");
    return Status::kInvalidArgument;
  }

  // Add vertex attribute buffer to our vector
  m_attributeBufferHandles.push_back(a_bufferHandle);

  return Status::kSuccess;
}

Status VertexArray::setIndexBuffer(FlurrHandle a_bufferHandle) {
  if (m_isArrayInitialized)
  {
    FLURR_LOG_ERROR("Unable to set index buffer; vertex array already created!");
    return Status::kInvalidState;
  }

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

VertexBuffer* VertexArray::getAttributeBuffer(std::size_t a_attributeIndex) const {
  if (a_attributeIndex >= m_attributeBufferHandles.size())
  {
    FLURR_LOG_ERROR("No vertex attribute buffer at index %u!", static_cast<uint32_t>(a_attributeIndex));
    return nullptr;
  }

  auto* renderer = FlurrCore::Get().getRenderer();
  return renderer->getVertexBuffer(m_attributeBufferHandles[a_attributeIndex]);
}

VertexBuffer* VertexArray::getIndexBuffer() const
{
  auto* renderer = FlurrCore::Get().getRenderer();
  return renderer->getVertexBuffer(m_indexBufferHandle);
}

Status VertexArray::initArray()
{
  if (m_isArrayInitialized)
  {
    FLURR_LOG_ERROR("Unable to create vertex array; already created!");
    return Status::kInvalidState;
  }

  if (getAttributeBuffersCount() <= 0)
  {
    FLURR_LOG_ERROR("Unable to create vertex array; no vertex buffers created yet!");
    return Status::kInvalidState;
  }

  if (!getIndexBuffer())
  {
    FLURR_LOG_ERROR("Unable to create vertex array; no index buffer created yet!");
    return Status::kInvalidState;
  }

  Status result = onInitArray();
  if (Status::kSuccess == result) m_isArrayInitialized = true;
  return result;
}

void VertexArray::destroyArray()
{
  // Delete vertex array
  onDestroyArray();
  m_attributeBufferHandles.clear();
  m_indexBufferHandle = INVALID_HANDLE;
  m_isArrayInitialized = false;
}

Status VertexArray::drawArray()
{
  if (!m_isArrayInitialized)
  {
    FLURR_LOG_ERROR("Unable to draw vertex array; not created yet!");
    return Status::kInvalidState;
  }

  return onDrawArray();
}

} // namespace flurr
