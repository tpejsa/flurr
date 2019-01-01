#include "flurr/renderer/VertexBuffer.h"
#include "flurr/FlurrLog.h"

namespace flurr
{

VertexBuffer::VertexBuffer(FlurrHandle a_bufferHandle)
  : m_bufferHandle(a_bufferHandle),
  m_bufferType(VertexBufferType::kVertexAttribute),
  m_dataSize(0),
  m_data(nullptr),
  m_dataUsage(VertexDataUsage::kStatic)
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

  m_bufferType = a_bufferType;
  m_dataUsage = a_dataUsage;
  m_dataSize = a_dataSize;
  m_data = a_data;
  m_attributeSize = a_attributeSize;
  return onInitBuffer();
}

Status VertexBuffer::initIndexBuffer(std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage) {
  return initBuffer(VertexBufferType::kIndex, a_dataSize, a_data, sizeof(uint32_t), a_dataUsage);
}

void VertexBuffer::destroyBuffer()
{
  onDestroyBuffer();

  // Set data pointer to null
  m_dataSize = 0;
  m_data = nullptr;
}

Status VertexBuffer::useBuffer()
{
  if (!m_data)
  {
    FLURR_LOG_ERROR("Unable to use vertex buffer; not created yet!");
    return Status::kInvalidState;
  }

  return onUseBuffer();
}

} // namespace flurr
