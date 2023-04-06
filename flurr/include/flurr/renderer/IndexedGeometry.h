#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/renderer/VertexBuffer.h"

#include <memory>
#include <vector>

namespace flurr
{

class FLURR_DLL_EXPORT IndexedGeometry
{
  friend class Renderer;

public:

  IndexedGeometry(FlurrHandle a_geometryHandle);
  IndexedGeometry(const IndexedGeometry&) = delete;
  IndexedGeometry(IndexedGeometry&&) = default;
  IndexedGeometry& operator=(const IndexedGeometry&) = delete;
  IndexedGeometry& operator=(IndexedGeometry&&) = default;
  ~IndexedGeometry() = default;

  FlurrHandle getGeometryHandle() const { return m_geometryHandle; }
  VertexBuffer* getAttributeBuffer(std::size_t a_attributeIndex) const;
  std::size_t getAttributeBuffersCount() const { return m_attributeBufferHandles.size(); }
  VertexBuffer* getIndexBuffer() const;
  bool isGeometryInitialized() const { return m_geometryInitialized; }

  GLuint getOGLVertexArrayObjectId() const { return m_oglVaoId; }

private:

  Status initGeometry(const std::vector<FlurrHandle>& a_attributeBufferHandles, FlurrHandle a_indexBufferHandle);
  void destroyGeometry();
  Status drawGeometry();
  Status addAttributeBuffer(FlurrHandle a_bufferHandle);
  Status setIndexBuffer(FlurrHandle a_bufferHandle);

  FlurrHandle m_geometryHandle;
  bool m_geometryInitialized;
  std::vector<FlurrHandle> m_attributeBufferHandles;
  FlurrHandle m_indexBufferHandle;

  GLuint m_oglVaoId;
};

} // namespace flurr
