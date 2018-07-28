#pragma once

#include "FlurrDefines.h"
#include "renderer/FlurrVertexBuffer.h"

#include <memory>
#include <vector>

namespace flurr {

class FLURR_DLL_EXPORT VertexArray {

public:

  VertexArray(VertexArrayHandle array_handle);
  VertexArray(const VertexArray&) = default;
  VertexArray(VertexArray&&) = default;
  VertexArray& operator=(const VertexArray&) = default;
  VertexArray& operator=(VertexArray&&) = default;
  virtual ~VertexArray() = default;

  Status CreateAttributeBuffer(void* data, std::size_t data_size, std::size_t attribute_size, VertexDataUsage data_usage = VertexDataUsage::kStatic);
  Status CreateIndexBuffer(uint32_t* data, std::size_t data_size, VertexDataUsage data_usage = VertexDataUsage::kStatic);
  inline VertexBuffer* GetAttributeBuffer(std::size_t attribute_index) const;
  inline std::size_t GetAttributeBuffersCount() const { attribute_buffers_.size(); }
  inline VertexBuffer* GetIndexBuffer() const { return index_buffer_.get(); }
  void DeleteArray();
  Status DrawArray();

private:

  virtual VertexBuffer* OnCreateBuffer() = 0;
  virtual void OnDeleteArray() = 0;
  virtual Status OnDrawArray() = 0;

  VertexArrayHandle array_handle_;
  std::vector<std::unique_ptr<VertexBuffer>> attribute_buffers_;
  std::unique_ptr<VertexBuffer> index_buffer_;
};

} // namespace flurr
