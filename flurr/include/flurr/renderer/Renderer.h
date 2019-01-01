#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/renderer/ShaderProgram.h"
#include "flurr/renderer/VertexArray.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace flurr
{

class FLURR_DLL_EXPORT FlurrRenderer
{

protected:

  FlurrRenderer();
  FlurrRenderer(const FlurrRenderer&) = delete;
  FlurrRenderer(FlurrRenderer&&) = delete;
  FlurrRenderer& operator=(const FlurrRenderer&) = delete;
  FlurrRenderer& operator=(FlurrRenderer&&) = delete;
  virtual ~FlurrRenderer();

public:

  Status init(const std::string& a_configPath = "flurr_renderer.cfg");
  void shutdown();
  Status update(float a_deltaTime);
  bool isInitialized() const { return m_initialized; }

  virtual void setViewport(int a_x, int a_y, uint32_t a_width, uint32_t a_height) = 0;

  FlurrHandle createShaderProgram();
  bool hasShaderProgram(FlurrHandle a_programHandle) const;
  ShaderProgram* getShaderProgram(FlurrHandle a_programHandle) const;
  void destroyShaderProgram(FlurrHandle a_programHandle);
  Status compileShader(FlurrHandle a_programHandle, ShaderType a_shaderType, FlurrHandle a_shaderResourceHandle);
  Status linkShaderProgram(FlurrHandle a_programHandle);
  Status useShaderProgram(FlurrHandle a_programHandle);
  FlurrHandle createVertexBuffer();
  bool hasVertexBuffer(FlurrHandle a_bufferHandle) const;
  VertexBuffer* getVertexBuffer(FlurrHandle a_bufferHandle) const;
  void destroyVertexBuffer(FlurrHandle a_bufferHandle);
  Status initVertexBuffer(FlurrHandle a_bufferHandle, VertexBufferType a_bufferType, std::size_t a_dataSize, void* a_data, std::size_t a_attributeSize, VertexDataUsage a_dataUsage = VertexDataUsage::kStatic);
  Status initIndexBuffer(FlurrHandle a_bufferHandle, std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage = VertexDataUsage::kStatic);
  Status useVertexBuffer(FlurrHandle a_bufferHandle);
  FlurrHandle createVertexArray();
  bool hasVertexArray(FlurrHandle a_arrayHandle) const;
  VertexArray* getVertexArray(FlurrHandle a_arrayHandle) const;
  void destroyVertexArray(FlurrHandle a_arrayHandle);
  Status initVertexArray(FlurrHandle a_arrayHandle, const std::vector<FlurrHandle>& a_attributeBufferHandles, FlurrHandle a_indexBufferHandle);
  Status drawVertexArray(FlurrHandle a_arrayHandle);

private:

  virtual Status onInit() = 0;
  virtual void onShutdown() = 0;
  virtual Status onUpdate(float a_deltaTime) = 0;

  virtual ShaderProgram* onCreateShaderProgram(FlurrHandle a_programHandle) = 0;
  virtual VertexBuffer* onCreateVertexBuffer(FlurrHandle a_bufferHandle) = 0;
  virtual VertexArray* onCreateVertexArray(FlurrHandle a_arrayHandle) = 0;

  bool m_initialized;

  // Shaders
  FlurrHandle m_nextShaderProgramHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<ShaderProgram>> m_shaderPrograms;
  // Vertex buffers
  FlurrHandle m_nextVertexBufferHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<VertexBuffer>> m_vertexBuffers;
  // Vertex arrays
  FlurrHandle m_nextVertexArrayHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<VertexArray>> m_vertexArrays;
};

} // namespace flurr
