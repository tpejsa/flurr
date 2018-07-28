#include "renderer/FlurrVertexArray.h"
#include "FlurrLog.h"

namespace flurr {

VertexArray::VertexArray(VertexArrayHandle array_handle)
  : array_handle_(array_handle) {
}

Status VertexArray::CreateAttributeBuffer(void* data, std::size_t data_size, std::size_t attribute_size, VertexDataUsage data_usage) {
  if (nullptr == data) {
    FLURR_LOG_ERROR("data cannot be null!");
    return Status::kNullArgument;
  }

  if (data_size <= 0) {
    FLURR_LOG_ERROR("data size must be > 0!");
    return Status::kInvalidArgument;
  }

  // Create vertex attribute buffer
  auto* vertex_buffer = OnCreateBuffer();
  if (!vertex_buffer) {
    FLURR_LOG_ERROR("Failed to create VertexBuffer object!");
    return Status::kFailed;
  }
  Status result = vertex_buffer->CreateBuffer(
    VertexBufferType::kVertexAttribute, data_size, data, data_usage);
  if (Status::kSuccess != result) {
    // Failed to initialize vertex buffer, delete created instance
    delete vertex_buffer;
    return result;
  }

  // Add vertex attribute buffer to our vector
  attribute_buffers_.emplace_back(std::unique_ptr<VertexBuffer>(vertex_buffer));

  return Status::kSuccess;
}

Status VertexArray::CreateIndexBuffer(uint32_t* data, std::size_t data_size, VertexDataUsage data_usage) {
  if (nullptr == data) {
    FLURR_LOG_ERROR("data cannot be null!");
    return Status::kNullArgument;
  }

  if (data_size <= 0) {
    FLURR_LOG_ERROR("data size must be > 0!");
    return Status::kInvalidArgument;
  }

  // Create index buffer
  auto* index_buffer = OnCreateBuffer();
  if (!index_buffer) {
    FLURR_LOG_ERROR("Failed to create VertexBuffer object!");
    return Status::kFailed;
  }
  Status result = index_buffer->CreateBuffer(
    VertexBufferType::kIndex, data_size, data, data_usage);
  if (Status::kSuccess != result) {
    // Failed to initialize vertex buffer, delete created instance
    delete index_buffer;
    return result;
  }

  // Set it as our current index buffer
  index_buffer_ = std::unique_ptr<VertexBuffer>(index_buffer);

  return Status::kSuccess;
}

inline VertexBuffer* VertexArray::GetAttributeBuffer(std::size_t attribute_index) const {
  if (attribute_index >= attribute_buffers_.size()) {
    FLURR_LOG_ERROR("No vertex attribute buffer at index %u!", static_cast<uint32_t>(attribute_index));
    return nullptr;
  }

  return attribute_buffers_[attribute_index].get();
}

void VertexArray::DeleteArray() {
  // Delete vertex buffers comprising the array
  for (std::size_t attribute_index = 0; attribute_index < attribute_buffers_.size(); ++attribute_index)
    attribute_buffers_[attribute_index]->DeleteBuffer();
  if (index_buffer_)
    index_buffer_->DeleteBuffer();

  // Delete vertex array
  OnDeleteArray();
}

Status VertexArray::DrawArray() {
  return OnDrawArray();
}

} // namespace flurr
