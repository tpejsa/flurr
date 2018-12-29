#pragma once
#include "flurr/FlurrDefines.h"

namespace flurr
{

enum class VertexBufferType
{
  kVertexAttribute,
  kIndex
};

enum class VertexDataUsage
{
  kStatic,
  kDynamic
};

class FLURR_DLL_EXPORT VertexBuffer
{

public:

  VertexBuffer(FlurrHandle a_bufferHandle);
  VertexBuffer(const VertexBuffer&) = default;
  VertexBuffer(VertexBuffer&&) = default;
  VertexBuffer& operator=(const VertexBuffer&) = default;
  VertexBuffer& operator=(VertexBuffer&&) = default;
  virtual ~VertexBuffer() = default;

  FlurrHandle getBufferHandle() const { return m_bufferHandle; }
  VertexBufferType getBufferType() const { return m_bufferType; }
  std::size_t getDataSize() const { return m_dataSize; }
  void* getData() const { return m_data; }
  std::size_t getAttributeSize() const { return m_attributeSize; }
  VertexDataUsage getDataUsage() const { return m_dataUsage; }
  Status createBuffer(VertexBufferType a_bufferType, std::size_t a_dataSize, void* a_data, std::size_t a_attributeSize, VertexDataUsage a_dataUsage = VertexDataUsage::kStatic);
  Status createIndexBuffer(std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage = VertexDataUsage::kStatic);
  void destroyBuffer();
  Status useBuffer();

private:

  virtual Status onCreateBuffer() = 0;
  virtual void onDestroyBuffer() = 0;
  virtual Status onUseBuffer() = 0;

  FlurrHandle m_bufferHandle;
  VertexBufferType m_bufferType;
  std::size_t m_dataSize;
  void* m_data;
  std::size_t m_attributeSize;
  VertexDataUsage m_dataUsage;
};

} // namespace flurr
