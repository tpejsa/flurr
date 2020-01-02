#include "flogger/OGLRenderer.h"
#include "flogger/OGLShaderProgram.h"
#include "flogger/OGLTexture.h"
#include "flogger/OGLVertexBuffer.h"
#include "flogger/OGLVertexArray.h"

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

FlurrOGLRenderer::FlurrOGLRenderer()
{
}

FlurrOGLRenderer::~FlurrOGLRenderer()
{
}

void FlurrOGLRenderer::setViewport(int a_x, int a_y, uint32_t a_width, uint32_t a_height)
{
  glViewport(a_x, a_y, a_width, a_height);
}

FlurrOGLRenderer& FlurrOGLRenderer::Get()
{
  static FlurrOGLRenderer flurrRenderer;
  return flurrRenderer;
}

Status FlurrOGLRenderer::onInit()
{
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

  return Status::kSuccess;
}

void FlurrOGLRenderer::onShutdown()
{
}

Status FlurrOGLRenderer::onUpdate(float a_deltaTime)
{
  // Clear buffers
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  return Status::kSuccess;
}

ShaderProgram* FlurrOGLRenderer::onCreateShaderProgram(FlurrHandle a_programHandle)
{
  return new OGLShaderProgram(a_programHandle);
}

Texture* FlurrOGLRenderer::onCreateTexture(FlurrHandle a_texHandle)
{
  return new OGLTexture(a_texHandle);
}

VertexBuffer* FlurrOGLRenderer::onCreateVertexBuffer(FlurrHandle a_bufferHandle)
{
  return new OGLVertexBuffer(a_bufferHandle);
}

VertexArray* FlurrOGLRenderer::onCreateVertexArray(FlurrHandle a_arrayHandle)
{
  return new OGLVertexArray(a_arrayHandle);
}

} // namespace flurr
