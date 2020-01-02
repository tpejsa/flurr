#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/renderer/ShaderProgram.h"
#include "flurr/renderer/Texture.h"
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

  Status createShaderProgram(FlurrHandle& a_programHandle);
  bool hasShaderProgram(FlurrHandle a_programHandle) const;
  ShaderProgram* getShaderProgram(FlurrHandle a_programHandle) const;
  void destroyShaderProgram(FlurrHandle a_programHandle);
  Status compileShader(FlurrHandle a_programHandle, ShaderType a_shaderType, FlurrHandle a_shaderResourceHandle);
  Status linkShaderProgram(FlurrHandle a_programHandle);
  Status useShaderProgram(FlurrHandle a_programHandle);
  Status createTexture(FlurrHandle& a_texHandle, FlurrHandle a_texResourceHandle, TextureWrapMode a_texWrapMode = TextureWrapMode::kRepeat, TextureMinFilterMode a_texMinFilterMode = TextureMinFilterMode::kLinearMipmapLinear, TextureMagFilterMode a_texMagFilterMode = TextureMagFilterMode::kLinear);
  bool hasTexture(FlurrHandle a_texHandle) const;
  Texture* getTexture(FlurrHandle a_texHandle) const;
  void destroyTexture(FlurrHandle a_texHandle);
  Status useTexture(FlurrHandle a_texHandle, TextureUnitIndex a_texUnit = 0);
  Status createVertexBuffer(FlurrHandle& a_bufferHandle, VertexBufferType a_bufferType, std::size_t a_dataSize, void* a_data, std::size_t a_attributeSize, VertexDataUsage a_dataUsage = VertexDataUsage::kStatic);
  Status createIndexBuffer(FlurrHandle& a_bufferHandle, std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage = VertexDataUsage::kStatic);
  bool hasVertexBuffer(FlurrHandle a_bufferHandle) const;
  VertexBuffer* getVertexBuffer(FlurrHandle a_bufferHandle) const;
  void destroyVertexBuffer(FlurrHandle a_bufferHandle);
  Status useVertexBuffer(FlurrHandle a_bufferHandle);
  Status createVertexArray(FlurrHandle& a_arrayHandle, const std::vector<FlurrHandle>& a_attributeBufferHandles, FlurrHandle a_indexBufferHandle);
  bool hasVertexArray(FlurrHandle a_arrayHandle) const;
  VertexArray* getVertexArray(FlurrHandle a_arrayHandle) const;
  void destroyVertexArray(FlurrHandle a_arrayHandle);
  Status drawVertexArray(FlurrHandle a_arrayHandle);

private:

  virtual Status onInit() = 0;
  virtual void onShutdown() = 0;
  virtual Status onUpdate(float a_deltaTime) = 0;

  virtual ShaderProgram* onCreateShaderProgram(FlurrHandle a_programHandle) = 0;
  virtual Texture* onCreateTexture(FlurrHandle a_texHandle) = 0;
  virtual VertexBuffer* onCreateVertexBuffer(FlurrHandle a_bufferHandle) = 0;
  virtual VertexArray* onCreateVertexArray(FlurrHandle a_arrayHandle) = 0;

  bool m_initialized;

  // Shaders
  FlurrHandle m_nextShaderProgramHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<ShaderProgram>> m_shaderPrograms;
  // Textures
  FlurrHandle m_nextTextureHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<Texture>> m_textures;
  // Vertex buffers
  FlurrHandle m_nextVertexBufferHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<VertexBuffer>> m_vertexBuffers;
  // Vertex arrays
  FlurrHandle m_nextVertexArrayHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<VertexArray>> m_vertexArrays;
};

} // namespace flurr
