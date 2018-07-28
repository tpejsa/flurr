#include "renderer/FlurrVertexBuffer.h"
#include "FlurrLog.h"

namespace flurr {

VertexBuffer::VertexBuffer()
  : buffer_type_(VertexBufferType::kVertexAttribute),
  data_size_(0),
  data_(nullptr),
  data_usage_(VertexDataUsage::kStatic) {
}

Status VertexBuffer::CreateBuffer(VertexBufferType buffer_type, std::size_t data_size, void* data, VertexDataUsage data_usage) {
  buffer_type_ = buffer_type;
  data_usage_ = data_usage;
  data_size_ = data_size;
  data_ = data;
  return OnCreateBuffer();
}

void VertexBuffer::DeleteBuffer() {
  OnDeleteBuffer();

  // Set data pointer to null
  data_size_ = 0;
  data_ = nullptr;
}

} // namespace flurr
