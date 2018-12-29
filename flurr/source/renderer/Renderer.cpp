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
    destroyVertexArray(vertexArrayKvp.first);
  m_vertexArrays.clear();
  for (auto&& vertexBufferKvp : m_vertexBuffers)
    destroyVertexBuffer(vertexBufferKvp.first);
  m_vertexBuffers.clear();
  for (auto&& shaderProgramKvp : m_shaderPrograms)
    destroyShaderProgram(shaderProgramKvp.first);
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
    return INVALID_OBJECT;
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

FlurrHandle FlurrRenderer::createVertexBuffer()
{
  // Create VertexBuffer instance
  FlurrHandle bufferHandle = m_nextVertexBufferHandle++;
  auto* vertexBuffer = onCreateVertexBuffer(bufferHandle);
  m_vertexBuffers.emplace(bufferHandle, std::unique_ptr<VertexBuffer>(vertexBuffer));

  return bufferHandle;
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

FlurrHandle FlurrRenderer::createVertexArray()
{
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return INVALID_OBJECT;
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

  if (vertexArray->isArrayCreated())
    vertexArray->destroyArray();
  m_vertexArrays.erase(a_arrayHandle);
}

} // namespace flurr
