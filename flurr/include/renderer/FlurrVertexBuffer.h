#pragma once
#include "FlurrDefines.h"

namespace flurr {

enum class VertexBufferType {
  kVertexAttribute,
  kIndex
};

enum class VertexDataUsage {
  kStatic,
  kDynamic
};

class FLURR_DLL_EXPORT VertexBuffer {

public:

  VertexBuffer();
  VertexBuffer(const VertexBuffer&) = default;
  VertexBuffer(VertexBuffer&&) = default;
  VertexBuffer& operator=(const VertexBuffer&) = default;
  VertexBuffer& operator=(VertexBuffer&&) = default;
  virtual ~VertexBuffer() = default;

  inline VertexBufferType GetBufferType() const { return buffer_type_; }
  inline VertexDataUsage GetDataUsage() const { return data_usage_; }
  inline std::size_t GetDataSize() const { return data_size_; }
  inline void* GetData() const { return data_; }
  Status CreateBuffer(VertexBufferType buffer_type, std::size_t data_size, void* data, VertexDataUsage data_usage = VertexDataUsage::kStatic);
  void DeleteBuffer();

private:

  virtual Status OnCreateBuffer() = 0;
  virtual Status OnDeleteBuffer() = 0;

  VertexBufferType buffer_type_;
  std::size_t data_size_;
  void* data_;
  VertexDataUsage data_usage_;
};

} // namespace flurr
