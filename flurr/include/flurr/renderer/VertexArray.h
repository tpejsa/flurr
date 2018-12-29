#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/renderer/VertexBuffer.h"

#include <memory>
#include <vector>

namespace flurr
{

class FLURR_DLL_EXPORT VertexArray
{

public:

  VertexArray(FlurrHandle a_arrayHandle);
  VertexArray(const VertexArray&) = default;
  VertexArray(VertexArray&&) = default;
  VertexArray& operator=(const VertexArray&) = default;
  VertexArray& operator=(VertexArray&&) = default;
  virtual ~VertexArray() = default;

  FlurrHandle getArrayHandle() const { return m_arrayHandle; }
  Status addAttributeBuffer(FlurrHandle a_bufferHandle);
  Status setIndexBuffer(FlurrHandle a_bufferHandle);
  VertexBuffer* getAttributeBuffer(std::size_t a_attributeIndex) const;
  std::size_t getAttributeBuffersCount() const { return m_attributeBufferHandles.size(); }
  VertexBuffer* getIndexBuffer() const;
  Status createArray();
  bool isArrayCreated() const { return m_isArrayCreated; }
  void destroyArray();
  Status drawArray();

private:

  virtual Status onCreateArray() = 0;
  virtual void onDestroyArray() = 0;
  virtual Status onDrawArray() = 0;

  FlurrHandle m_arrayHandle;
  bool m_isArrayCreated;
  std::vector<FlurrHandle> m_attributeBufferHandles;
  FlurrHandle m_indexBufferHandle;
};

} // namespace flurr
