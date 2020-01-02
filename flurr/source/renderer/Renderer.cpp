#include "flurr/renderer/Renderer.h"
#include "flurr/FlurrCore.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/ConfigFile.h"

namespace flurr
{

FlurrRenderer::FlurrRenderer()
  : m_initialized(false),
  m_nextShaderProgramHandle(1),
  m_nextVertexBufferHandle(1),
  m_nextVertexArrayHandle(1)
{
}

FlurrRenderer::~FlurrRenderer()
{
  if (m_initialized)
    shutdown();
}

Status FlurrRenderer::init(const std::string& a_configPath)
{
  FLURR_LOG_INFO("Initializing flurr renderer...");

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

  FLURR_LOG_INFO("flurr renderer initialized.");
  m_initialized = true;
  return result;
}

void FlurrRenderer::shutdown()
{
  FLURR_LOG_INFO("Shutting down flurr renderer...");
  if (!m_initialized)
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
  for (auto&& vertexBufferKvp : m_vertexBuffers)
    if (vertexBufferKvp.second->getData())
      vertexBufferKvp.second->destroyBuffer();
  m_vertexBuffers.clear();
  for (auto&& shaderProgramKvp : m_shaderPrograms)
    if (shaderProgramKvp.second->getProgramState() != ShaderProgramState::kDestroyed)
      shaderProgramKvp.second->destroyProgram();
  m_shaderPrograms.clear();

  // Flag renderer as uninitialized
  m_initialized = false;
  FLURR_LOG_INFO("flurr renderer shutdown complete.");
}

Status FlurrRenderer::update(float a_deltaTime)
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  return onUpdate(a_deltaTime);
}

Status FlurrRenderer::createShaderProgram(FlurrHandle& a_programHandle)
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create ShaderProgram instance
  a_programHandle = m_nextShaderProgramHandle++;
  m_shaderPrograms.emplace(a_programHandle, std::unique_ptr<ShaderProgram>(onCreateShaderProgram(a_programHandle)));

  return Status::kSuccess;
}

bool FlurrRenderer::hasShaderProgram(FlurrHandle a_programHandle) const
{
  return m_shaderPrograms.find(a_programHandle) != m_shaderPrograms.end();
}

ShaderProgram* FlurrRenderer::getShaderProgram(FlurrHandle a_programHandle) const
{
  auto&& shaderProgramIt = m_shaderPrograms.find(a_programHandle);
  return m_shaderPrograms.end() == shaderProgramIt ?
    nullptr : shaderProgramIt->second.get();
}

void FlurrRenderer::destroyShaderProgram(FlurrHandle a_programHandle)
{
  if (!m_initialized)
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
}

Status FlurrRenderer::compileShader(FlurrHandle a_programHandle, ShaderType a_shaderType, FlurrHandle a_shaderResourceHandle)
{
  if (!m_initialized)
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

Status FlurrRenderer::linkShaderProgram(FlurrHandle a_programHandle)
{
  if (!m_initialized)
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

Status FlurrRenderer::useShaderProgram(FlurrHandle a_programHandle)
{
  if (!m_initialized)
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

Status FlurrRenderer::createTexture(FlurrHandle& a_texHandle, FlurrHandle a_texResourceHandle, TextureWrapMode a_texWrapMode, TextureMinFilterMode a_texMinFilterMode, TextureMagFilterMode a_texMagFilterMode)
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create Texture instance
  a_texHandle = m_nextTextureHandle++;
  auto* texture = onCreateTexture(a_texHandle);
  m_textures.emplace(a_texHandle, std::unique_ptr<Texture>(texture));

  // Initialize texture with data
  return texture->initTexture(a_texResourceHandle, a_texWrapMode, a_texMinFilterMode, a_texMagFilterMode);
}

bool FlurrRenderer::hasTexture(FlurrHandle a_texHandle) const
{
  return m_textures.find(a_texHandle) != m_textures.end();
}

Texture* FlurrRenderer::getTexture(FlurrHandle a_texHandle) const
{
  auto&& texIt = m_textures.find(a_texHandle);
  return m_textures.end() == texIt ? nullptr : texIt->second.get();
}

void FlurrRenderer::destroyTexture(FlurrHandle a_texHandle)
{
  if (!m_initialized)
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
}

Status FlurrRenderer::useTexture(FlurrHandle a_texHandle, TextureUnitIndex a_texUnit)
{
  if (!m_initialized)
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

Status FlurrRenderer::createVertexBuffer(FlurrHandle& a_bufferHandle, VertexBufferType a_bufferType, std::size_t a_dataSize, void* a_data, std::size_t a_attributeSize, VertexDataUsage a_dataUsage)
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create VertexBuffer instance
  a_bufferHandle = m_nextVertexBufferHandle++;
  auto* vertexBuffer = onCreateVertexBuffer(a_bufferHandle);
  m_vertexBuffers.emplace(a_bufferHandle, std::unique_ptr<VertexBuffer>(vertexBuffer));

  // Initialize vertex buffer with data
  return vertexBuffer->initBuffer(a_bufferType, a_dataSize, a_data, a_attributeSize, a_dataUsage);
}

Status FlurrRenderer::createIndexBuffer(FlurrHandle& a_bufferHandle, std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage)
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create VertexBuffer instance
  a_bufferHandle = m_nextVertexBufferHandle++;
  auto* vertexBuffer = onCreateVertexBuffer(a_bufferHandle);
  m_vertexBuffers.emplace(a_bufferHandle, std::unique_ptr<VertexBuffer>(vertexBuffer));

  // Initialize index buffer with data
  return vertexBuffer->initIndexBuffer(a_dataSize, a_data, a_dataUsage);
}

bool FlurrRenderer::hasVertexBuffer(FlurrHandle a_bufferHandle) const
{
  return m_vertexBuffers.find(a_bufferHandle) != m_vertexBuffers.end();
}

VertexBuffer* FlurrRenderer::getVertexBuffer(FlurrHandle a_bufferHandle) const
{
  auto&& bufferIt = m_vertexBuffers.find(a_bufferHandle);
  return m_vertexBuffers.end() == bufferIt ? nullptr : bufferIt->second.get();
}

void FlurrRenderer::destroyVertexBuffer(FlurrHandle a_bufferHandle)
{
  if (!m_initialized)
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
}

Status FlurrRenderer::useVertexBuffer(FlurrHandle a_bufferHandle)
{
  if (!m_initialized)
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

Status FlurrRenderer::createVertexArray(FlurrHandle& a_arrayHandle, const std::vector<FlurrHandle>& a_attributeBufferHandles, FlurrHandle a_indexBufferHandle)
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create VertexArray instance
  a_arrayHandle = m_nextVertexArrayHandle++;
  auto* vertexArray = onCreateVertexArray(a_arrayHandle);
  m_vertexArrays.emplace(a_arrayHandle, std::unique_ptr<VertexArray>(vertexArray));

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

bool FlurrRenderer::hasVertexArray(FlurrHandle a_arrayHandle) const
{
  return m_vertexArrays.find(a_arrayHandle) != m_vertexArrays.end();
}

VertexArray* FlurrRenderer::getVertexArray(FlurrHandle a_arrayHandle) const
{
  auto&& arrayIt = m_vertexArrays.find(a_arrayHandle);
  return m_vertexArrays.end() == arrayIt ?
    nullptr : arrayIt->second.get();
}

void FlurrRenderer::destroyVertexArray(FlurrHandle a_arrayHandle)
{
  if (!m_initialized)
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
}

Status FlurrRenderer::drawVertexArray(FlurrHandle a_arrayHandle)
{
  if (!m_initialized)
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
