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

FlurrHandle FlurrRenderer::createShaderProgram()
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return INVALID_HANDLE;
  }

  // Create ShaderProgram instance
  FlurrHandle programHandle = m_nextShaderProgramHandle++;
  m_shaderPrograms.emplace(programHandle, std::unique_ptr<ShaderProgram>(onCreateShaderProgram(programHandle)));
  return programHandle;
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

FlurrHandle FlurrRenderer::createVertexBuffer()
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return INVALID_HANDLE;
  }

  // Create VertexBuffer instance
  FlurrHandle bufferHandle = m_nextVertexBufferHandle++;
  auto* vertexBuffer = onCreateVertexBuffer(bufferHandle);
  m_vertexBuffers.emplace(bufferHandle, std::unique_ptr<VertexBuffer>(vertexBuffer));

  return bufferHandle;
}

bool FlurrRenderer::hasVertexBuffer(FlurrHandle a_bufferHandle) const
{
  return m_vertexBuffers.find(a_bufferHandle) != m_vertexBuffers.end();
}

VertexBuffer* FlurrRenderer::getVertexBuffer(FlurrHandle a_bufferHandle) const
{
  auto&& bufferIt = m_vertexBuffers.find(a_bufferHandle);
  return m_vertexBuffers.end() == bufferIt ?
    nullptr : bufferIt->second.get();
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

  if (vertexBuffer->getData())
    vertexBuffer->destroyBuffer();
  m_vertexBuffers.erase(a_bufferHandle);
}

Status FlurrRenderer::initVertexBuffer(FlurrHandle a_bufferHandle, VertexBufferType a_bufferType, std::size_t a_dataSize, void* a_data, std::size_t a_attributeSize, VertexDataUsage a_dataUsage)
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

  return vertexBuffer->initBuffer(a_bufferType, a_dataSize, a_data, a_attributeSize, a_dataUsage);
}

Status FlurrRenderer::initIndexBuffer(FlurrHandle a_bufferHandle, std::size_t a_dataSize, void* a_data, VertexDataUsage a_dataUsage)
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

  return vertexBuffer->initIndexBuffer(a_dataSize, a_data, a_dataUsage);
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

  return vertexBuffer->useBuffer();
}

FlurrHandle FlurrRenderer::createVertexArray()
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return INVALID_HANDLE;
  }

  // Create VertexArray instance
  FlurrHandle arrayHandle = m_nextVertexArrayHandle++;
  m_vertexArrays.emplace(arrayHandle, std::unique_ptr<VertexArray>(onCreateVertexArray(arrayHandle)));
  return arrayHandle;
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

  if (vertexArray->isArrayInitialized())
    vertexArray->destroyArray();
  m_vertexArrays.erase(a_arrayHandle);
}

Status FlurrRenderer::initVertexArray(FlurrHandle a_arrayHandle, const std::vector<FlurrHandle>& a_attributeBufferHandles, FlurrHandle a_indexBufferHandle)
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

  return vertexArray->drawArray();
}

} // namespace flurr
