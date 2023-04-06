#pragma once

#include "flurr/FlurrDefines.h"

#include <GL/glew.h>

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
  friend class Renderer;

public:

  VertexBuffer(FlurrHandle a_bufferHandle);
  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer(VertexBuffer&&) = default;
  VertexBuffer& operator=(const VertexBuffer&) = delete;
  VertexBuffer& operator=(VertexBuffer&&) = default;
  ~VertexBuffer() = default;

  FlurrHandle getBufferHandle() const { return m_bufferHandle; }
  VertexBufferType getBufferType() const { return m_bufferType; }
  std::size_t getDataSize() const { return m_dataSize; }
  std::size_t getAttributeSize() const { return m_attributeSize; }
  VertexDataUsage getDataUsage() const { return m_dataUsage; }
  bool isCreated() const { return m_dataSize > 0; }

  GLuint getOGLVertexBufferObjectId() const { return m_oglVboId; }

private:

  Status initBuffer(VertexBufferType a_bufferType, std::size_t a_dataSize, void* a_data, std::size_t a_attributeSize, VertexDataUsage a_dataUsage = VertexDataUsage::kStatic);
  Status initIndexBuffer(std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage = VertexDataUsage::kStatic);
  void destroyBuffer();
  Status useBuffer();

  FlurrHandle m_bufferHandle;
  VertexBufferType m_bufferType;
  std::size_t m_dataSize;
  std::size_t m_attributeSize;
  VertexDataUsage m_dataUsage;

  GLuint m_oglVboId;
};

} // namespace flurr
