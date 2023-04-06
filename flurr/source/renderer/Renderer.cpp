#include "flurr/renderer/Renderer.h"
#include "flurr/FlurrCore.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/ConfigFile.h"

#include <GL/glew.h>

#include <algorithm>

namespace flurr
{

void GLAPIENTRY OGLDebugMessageCallback(GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam) {
  FLURR_LOG_DEBUG("OGL 0x%x type = 0x%x, severity = 0x%x, message = %s",
    source, type, severity, message);
}

Renderer::Renderer()
  : m_initialized(false),
  m_nextShaderProgramHandle(1),
  m_nextTextureHandle(1),
  m_nextVertexBufferHandle(1),
  m_nextIndexedGeometryHandle(1)
{
}

Renderer::~Renderer()
{
  if (isInitialized())
    shutdown();
}

Status Renderer::init()
{
  FLURR_LOG_INFO("Initializing flurr renderer...");
  if (isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer already initialized!");
    return Status::kSuccess;
  }

  // Initialize GLEW
  GLenum glewResult = glewInit();
  if (GLEW_OK != glewResult)
  {
    FLURR_LOG_ERROR("Failed to initialize GLEW (error %u)!", glewResult);
    return Status::kFailed;
  }

  // Register debug callback
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(OGLDebugMessageCallback, 0);

  // Print OpenGL capabilities
  int numAttributes = 0;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
  FLURR_LOG_INFO("OpenGL info:\nNumber of attributes: %d", numAttributes);

  m_initialized = true;
  FLURR_LOG_INFO("flurr renderer initialized.");
  return Status::kSuccess;
}

void Renderer::shutdown()
{
  FLURR_LOG_INFO("Shutting down flurr renderer...");
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  // Destroy renderer resources
  for (auto&& geometryKvp : m_indexedGeometries)
    if (geometryKvp.second->isGeometryInitialized())
      geometryKvp.second->destroyGeometry();
  m_indexedGeometries.clear();
  m_indexedGeometryHandles.clear();
  for (auto&& vertexBufferKvp : m_vertexBuffers)
    if (vertexBufferKvp.second->isCreated())
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

  // Clear buffers
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  return Status::kSuccess;
}

void Renderer::setViewport(int a_x, int a_y, uint32_t a_width, uint32_t a_height)
{
  glViewport(a_x, a_y, a_width, a_height);
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
  m_shaderPrograms.emplace(a_programHandle, std::make_unique<ShaderProgram>(a_programHandle));
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
  m_textures.emplace(a_texHandle, std::make_unique<Texture>(a_texHandle));
  m_textureHandles.push_back(a_texHandle);

  // Initialize texture with data
  auto result = getTexture(a_texHandle)->initTexture(a_texResourceHandle, a_texWrapMode, a_texMinFilterMode, a_texMagFilterMode);
  if (result != Status::kSuccess)
  {
    // Failed to create the texture, clean up
    m_textures.erase(a_texHandle);
    m_textureHandles.pop_back();
    a_texHandle = INVALID_HANDLE;
  }

  return result;
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
  m_vertexBuffers.emplace(a_bufferHandle, std::make_unique<VertexBuffer>(a_bufferHandle));
  m_vertexBufferHandles.push_back(a_bufferHandle);

  // Initialize vertex buffer with data
  auto result = getVertexBuffer(a_bufferHandle)->initBuffer(a_bufferType, a_dataSize, a_data, a_attributeSize, a_dataUsage);
  if (result != Status::kSuccess)
  {
    // Failed to create vertex buffer, clean up
    m_vertexBuffers.erase(a_bufferHandle);
    m_indexedGeometryHandles.pop_back();
    a_bufferHandle = INVALID_HANDLE;    
  }
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
  m_vertexBuffers.emplace(a_bufferHandle, std::make_unique<VertexBuffer>(a_bufferHandle));
  m_vertexBufferHandles.push_back(a_bufferHandle);

  // Initialize index buffer with data
  return getVertexBuffer(a_bufferHandle)->initIndexBuffer(a_dataSize, a_data, a_dataUsage);
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
  if (vertexBuffer->isCreated())
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

Status Renderer::createIndexedGeometry(FlurrHandle& a_geometryHandle, const std::vector<FlurrHandle>& a_attributeBufferHandles, FlurrHandle a_indexBufferHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Create IndexedGeometry instance
  a_geometryHandle = GenerateHandle(m_nextIndexedGeometryHandle, [this](FlurrHandle a_h) { return hasIndexedGeometry(a_h); });
  m_indexedGeometries.emplace(a_geometryHandle, std::make_unique<IndexedGeometry>(a_geometryHandle));
  m_indexedGeometryHandles.push_back(a_geometryHandle);

  // Initialize indexed geometry with vertex and index buffers
  auto* geometry = getIndexedGeometry(a_geometryHandle);
  auto result = geometry->initGeometry(a_attributeBufferHandles, a_indexBufferHandle);
  if (result != Status::kSuccess)
  {
    // Failed to create the indexed geometry, clean up
    m_indexedGeometries.erase(a_geometryHandle);
    m_indexedGeometryHandles.pop_back();
    a_geometryHandle = INVALID_HANDLE;
  }

  return result;
}

void Renderer::destroyIndexedGeometry(FlurrHandle a_geometryHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  // Get IndexedGeometry object
  auto* geometry = getIndexedGeometry(a_geometryHandle);
  if (!geometry)
  {
    FLURR_LOG_WARN("No IndexedGeometry with handle %u!", a_geometryHandle);
    return;
  }

  // Destroy indexed geometry
  if (geometry->isGeometryInitialized())
    geometry->destroyGeometry();
  m_indexedGeometries.erase(a_geometryHandle);
  m_indexedGeometryHandles.erase(std::remove(m_indexedGeometryHandles.begin(), m_indexedGeometryHandles.end(), a_geometryHandle), m_indexedGeometryHandles.end());
}

bool Renderer::hasIndexedGeometry(FlurrHandle a_geometryHandle) const
{
  return m_indexedGeometries.find(a_geometryHandle) != m_indexedGeometries.end();
}

IndexedGeometry* Renderer::getIndexedGeometry(FlurrHandle a_geometryHandle) const
{
  auto&& arrayIt = m_indexedGeometries.find(a_geometryHandle);
  return m_indexedGeometries.end() == arrayIt ?
    nullptr : arrayIt->second.get();
}

IndexedGeometry* Renderer::getIndexedGeometryByIndex(std::size_t a_geometryIndex) const
{
  return a_geometryIndex < getIndexedGeometryCount() ? getIndexedGeometry(m_indexedGeometryHandles[a_geometryIndex]) : nullptr;
}

Status Renderer::drawIndexedGeometry(FlurrHandle a_geometryHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kInvalidState;
  }

  // Get IndexedGeometry object
  auto* geometry = getIndexedGeometry(a_geometryHandle);
  if (!geometry)
  {
    FLURR_LOG_WARN("No IndexedGeometry with handle %u!", a_geometryHandle);
    return Status::kInvalidArgument;
  }

  // Draw indexed geometry
  return geometry->drawGeometry();
}

} // namespace flurr
