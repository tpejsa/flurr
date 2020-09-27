#include "flurr/renderer/Renderer.h"
#include "flurr/FlurrCore.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/ConfigFile.h"

#include <algorithm>

namespace flurr
{

Renderer::Renderer()
  : m_initialized(false),
  m_nextShaderProgramHandle(1),
  m_nextVertexBufferHandle(1),
  m_nextVertexArrayHandle(1)
{
}

Renderer::~Renderer()
{
  if (isInitialized())
    shutdown();
}

Status Renderer::init(const std::string& a_configPath)
{
  FLURR_LOG_INFO("Initializing flurr renderer...");
  if (isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer already initialized!");
    return Status::kSuccess;
  }

  if (!FlurrCore::Get().isInitialized())
  {
    FLURR_LOG_ERROR("Failed to initialize flurr renderer; flurr core not initialized!");
    return Status::kNotInitialized;
  }

  // Make sure this renderer is set in FlurrCore
  FlurrCore::Get().setRenderer(this);

  // Initialize renderer
  Status result = onInit();
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to initialize flurr renderer!");
    return result;
  }

  m_initialized = true;
  FLURR_LOG_INFO("flurr renderer initialized.");
  return result;
}

void Renderer::shutdown()
{
  FLURR_LOG_INFO("Shutting down flurr renderer...");
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  onShutdown();

  // Destroy renderer resources
  for (auto&& vertexArrayKvp : m_vertexArrays)
    if (vertexArrayKvp.second->isArrayInitialized())
      vertexArrayKvp.second->destroyArray();
  m_vertexArrays.clear();
  m_vertexArrayHandles.clear();
  for (auto&& vertexBufferKvp : m_vertexBuffers)
    if (vertexBufferKvp.second->getData())
      vertexBufferKvp.second->destroyBuffer();
  m_vertexBuffers.clear();
  m_vertexBufferHandles.clear();
  for (auto&& shaderProgramKvp : m_shaderPrograms)
    if (shaderProgramKvp.second->getProgramState() != ShaderProgramState::kDestroyed)
      shaderProgramKvp.second->destroyProgram();
  m_shaderPrograms.clear();
  m_shaderProgramHandles.clear();

  // Flag renderer as uninitialized
  m_initialized = false;
  FLURR_LOG_INFO("flurr renderer shutdown complete.");
}

Status Renderer::update(float a_deltaTime)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  return onUpdate(a_deltaTime);
}

Status Renderer::createShaderProgram(FlurrHandle& a_programHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create ShaderProgram instance
  a_programHandle = GenerateHandle(m_nextShaderProgramHandle, [this](FlurrHandle a_h) { return hasShaderProgram(a_h); });
  m_shaderPrograms.emplace(a_programHandle, std::unique_ptr<ShaderProgram>(onCreateShaderProgram(a_programHandle)));
  m_shaderProgramHandles.push_back(a_programHandle);

  return Status::kSuccess;
}

void Renderer::destroyShaderProgram(FlurrHandle a_programHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  // Get ShaderProgram object
  auto* shaderProgram = getShaderProgram(a_programHandle);
  if (!shaderProgram)
  {
    FLURR_LOG_WARN("No ShaderProgram with handle %u!", a_programHandle);
    return;
  }

  if (shaderProgram->getProgramState() != ShaderProgramState::kDestroyed)
    shaderProgram->destroyProgram();
  m_shaderPrograms.erase(a_programHandle);
  m_shaderProgramHandles.erase(std::remove(m_shaderProgramHandles.begin(), m_shaderProgramHandles.end(), a_programHandle), m_shaderProgramHandles.end());
}

bool Renderer::hasShaderProgram(FlurrHandle a_programHandle) const
{
  return m_shaderPrograms.find(a_programHandle) != m_shaderPrograms.end();
}

ShaderProgram* Renderer::getShaderProgram(FlurrHandle a_programHandle) const
{
  auto&& shaderProgramIt = m_shaderPrograms.find(a_programHandle);
  return m_shaderPrograms.end() == shaderProgramIt ?
    nullptr : shaderProgramIt->second.get();
}

ShaderProgram* Renderer::getShaderProgramByIndex(std::size_t a_programIndex) const
{
  return a_programIndex < m_shaderProgramHandles.size() ? getShaderProgram(m_shaderProgramHandles[a_programIndex]) : nullptr;
}

Status Renderer::compileShader(FlurrHandle a_programHandle, ShaderType a_shaderType, FlurrHandle a_shaderResourceHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kInvalidState;
  }

  // Get ShaderProgram object
  auto* shaderProgram = getShaderProgram(a_programHandle);
  if (!shaderProgram)
  {
    FLURR_LOG_WARN("No ShaderProgram with handle %u!", a_programHandle);
    return Status::kInvalidArgument;
  }

  return shaderProgram->compileShader(a_shaderType, a_shaderResourceHandle);
}

Status Renderer::linkShaderProgram(FlurrHandle a_programHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kInvalidState;
  }

  // Get ShaderProgram object
  auto* shaderProgram = getShaderProgram(a_programHandle);
  if (!shaderProgram)
  {
    FLURR_LOG_WARN("No ShaderProgram with handle %u!", a_programHandle);
    return Status::kInvalidArgument;
  }

  return shaderProgram->linkProgram();
}

Status Renderer::useShaderProgram(FlurrHandle a_programHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kInvalidState;
  }

  // Get ShaderProgram object
  auto* shaderProgram = getShaderProgram(a_programHandle);
  if (!shaderProgram)
  {
    FLURR_LOG_WARN("No ShaderProgram with handle %u!", a_programHandle);
    return Status::kInvalidArgument;
  }

  return shaderProgram->useProgram();
}

Status Renderer::createTexture(FlurrHandle& a_texHandle, FlurrHandle a_texResourceHandle, TextureWrapMode a_texWrapMode, TextureMinFilterMode a_texMinFilterMode, TextureMagFilterMode a_texMagFilterMode)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create Texture instance
  a_texHandle = GenerateHandle(m_nextTextureHandle, [this](FlurrHandle a_h) { return hasTexture(a_h); });
  auto* texture = onCreateTexture(a_texHandle);
  m_textures.emplace(a_texHandle, std::unique_ptr<Texture>(texture));
  m_textureHandles.push_back(a_texHandle);

  // Initialize texture with data
  return texture->initTexture(a_texResourceHandle, a_texWrapMode, a_texMinFilterMode, a_texMagFilterMode);
}

void Renderer::destroyTexture(FlurrHandle a_texHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  // Get Texture object
  auto* texture = getTexture(a_texHandle);
  if (!texture)
  {
    FLURR_LOG_WARN("No Texture with handle %u!", a_texHandle);
    return;
  }

  // Destroy vertex buffer
  if (INVALID_HANDLE != texture->getResourceHandle())
    texture->destroyTexture();
  m_textures.erase(a_texHandle);
  m_textureHandles.erase(std::remove(m_textureHandles.begin(), m_textureHandles.end(), a_texHandle), m_textureHandles.end());
}

bool Renderer::hasTexture(FlurrHandle a_texHandle) const
{
  return m_textures.find(a_texHandle) != m_textures.end();
}

Texture* Renderer::getTexture(FlurrHandle a_texHandle) const
{
  auto&& texIt = m_textures.find(a_texHandle);
  return m_textures.end() == texIt ? nullptr : texIt->second.get();
}

Texture* Renderer::getTextureByIndex(std::size_t a_texIndex) const
{
  return a_texIndex < getTextureCount() ? getTexture(m_textureHandles[a_texIndex]) : nullptr;
}

Status Renderer::useTexture(FlurrHandle a_texHandle, TextureUnitIndex a_texUnit)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kInvalidState;
  }

  // Get Texture object
  auto* texture = getTexture(a_texHandle);
  if (!texture)
  {
    FLURR_LOG_WARN("No Texture with handle %u!", a_texHandle);
    return Status::kInvalidArgument;
  }

  // Use as current texture
  return texture->useTexture(a_texUnit);
}

Status Renderer::createVertexBuffer(FlurrHandle& a_bufferHandle, VertexBufferType a_bufferType, std::size_t a_dataSize, void* a_data, std::size_t a_attributeSize, VertexDataUsage a_dataUsage)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create VertexBuffer instance
  a_bufferHandle = GenerateHandle(m_nextVertexBufferHandle, [this](FlurrHandle a_h) { return hasVertexBuffer(a_h); });
  auto* vertexBuffer = onCreateVertexBuffer(a_bufferHandle);
  m_vertexBuffers.emplace(a_bufferHandle, std::unique_ptr<VertexBuffer>(vertexBuffer));
  m_vertexBufferHandles.push_back(a_bufferHandle);

  // Initialize vertex buffer with data
  return vertexBuffer->initBuffer(a_bufferType, a_dataSize, a_data, a_attributeSize, a_dataUsage);
}

Status Renderer::createIndexBuffer(FlurrHandle& a_bufferHandle, std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create VertexBuffer instance
  a_bufferHandle = GenerateHandle(m_nextVertexBufferHandle, [this](FlurrHandle a_h) { return hasVertexBuffer(a_h); });
  auto* vertexBuffer = onCreateVertexBuffer(a_bufferHandle);
  m_vertexBuffers.emplace(a_bufferHandle, std::unique_ptr<VertexBuffer>(vertexBuffer));
  m_vertexBufferHandles.push_back(a_bufferHandle);

  // Initialize index buffer with data
  return vertexBuffer->initIndexBuffer(a_dataSize, a_data, a_dataUsage);
}

void Renderer::destroyVertexBuffer(FlurrHandle a_bufferHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  // Get VertexBuffer object
  auto* vertexBuffer = getVertexBuffer(a_bufferHandle);
  if (!vertexBuffer)
  {
    FLURR_LOG_WARN("No VertexBuffer with handle %u!", a_bufferHandle);
    return;
  }

  // Destroy vertex buffer
  if (vertexBuffer->getData())
    vertexBuffer->destroyBuffer();
  m_vertexBuffers.erase(a_bufferHandle);
  m_vertexBufferHandles.erase(std::remove(m_vertexBufferHandles.begin(), m_vertexBufferHandles.end(), a_bufferHandle), m_vertexBufferHandles.end());
}

bool Renderer::hasVertexBuffer(FlurrHandle a_bufferHandle) const
{
  return m_vertexBuffers.find(a_bufferHandle) != m_vertexBuffers.end();
}

VertexBuffer* Renderer::getVertexBuffer(FlurrHandle a_bufferHandle) const
{
  auto&& bufferIt = m_vertexBuffers.find(a_bufferHandle);
  return m_vertexBuffers.end() == bufferIt ? nullptr : bufferIt->second.get();
}

VertexBuffer* Renderer::getVertexBufferByIndex(std::size_t a_bufferIndex) const
{
  return a_bufferIndex < getVertexBufferCount() ? getVertexBuffer(m_vertexBufferHandles[a_bufferIndex]) : nullptr;
}

Status Renderer::useVertexBuffer(FlurrHandle a_bufferHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kInvalidState;
  }

  // Get VertexBuffer object
  auto* vertexBuffer = getVertexBuffer(a_bufferHandle);
  if (!vertexBuffer)
  {
    FLURR_LOG_WARN("No VertexBuffer with handle %u!", a_bufferHandle);
    return Status::kInvalidArgument;
  }

  // Use as current vertex buffer
  return vertexBuffer->useBuffer();
}

Status Renderer::createVertexArray(FlurrHandle& a_arrayHandle, const std::vector<FlurrHandle>& a_attributeBufferHandles, FlurrHandle a_indexBufferHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create VertexArray instance
  a_arrayHandle = GenerateHandle(m_nextVertexArrayHandle, [this](FlurrHandle a_h) { return hasVertexArray(a_h); });
  auto* vertexArray = onCreateVertexArray(a_arrayHandle);
  m_vertexArrays.emplace(a_arrayHandle, std::unique_ptr<VertexArray>(vertexArray));
  m_vertexArrayHandles.push_back(a_arrayHandle);

  // Add attribute buffers to the vertex array
  Status result;
  for (auto attributeBufferHandle : a_attributeBufferHandles)
  {
    result = vertexArray->addAttributeBuffer(attributeBufferHandle);
    if (Status::kSuccess != result)
      return result;
  }
  result = vertexArray->setIndexBuffer(a_indexBufferHandle);
  if (Status::kSuccess != result)
    return result;

  return vertexArray->initArray();
}

void Renderer::destroyVertexArray(FlurrHandle a_arrayHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  // Get VertexArray object
  auto* vertexArray = getVertexArray(a_arrayHandle);
  if (!vertexArray)
  {
    FLURR_LOG_WARN("No VertexArray with handle %u!", a_arrayHandle);
    return;
  }

  // Destroy vertex array
  if (vertexArray->isArrayInitialized())
    vertexArray->destroyArray();
  m_vertexArrays.erase(a_arrayHandle);
  m_vertexArrayHandles.erase(std::remove(m_vertexArrayHandles.begin(), m_vertexArrayHandles.end(), a_arrayHandle), m_vertexArrayHandles.end());
}

bool Renderer::hasVertexArray(FlurrHandle a_arrayHandle) const
{
  return m_vertexArrays.find(a_arrayHandle) != m_vertexArrays.end();
}

VertexArray* Renderer::getVertexArray(FlurrHandle a_arrayHandle) const
{
  auto&& arrayIt = m_vertexArrays.find(a_arrayHandle);
  return m_vertexArrays.end() == arrayIt ?
    nullptr : arrayIt->second.get();
}

VertexArray* Renderer::getVertexArrayByIndex(std::size_t a_arrayIndex) const
{
  return a_arrayIndex < getVertexArrayCount() ? getVertexArray(m_vertexArrayHandles[a_arrayIndex]) : nullptr;
}

Status Renderer::drawVertexArray(FlurrHandle a_arrayHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kInvalidState;
  }

  // Get VertexArray object
  auto* vertexArray = getVertexArray(a_arrayHandle);
  if (!vertexArray)
  {
    FLURR_LOG_WARN("No VertexArray with handle %u!", a_arrayHandle);
    return Status::kInvalidArgument;
  }

  // Draw vertex array
  return vertexArray->drawArray();
}

} // namespace flurr
