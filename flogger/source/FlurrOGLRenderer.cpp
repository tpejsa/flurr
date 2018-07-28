#include "FlurrOGLRenderer.h"
#include "FlurrOGLShaderProgram.h"

namespace flurr {

FlurrOGLRenderer::FlurrOGLRenderer() {
}

FlurrOGLRenderer::~FlurrOGLRenderer() {
}

void FlurrOGLRenderer::SetViewport(int x, int y, uint32_t width, uint32_t height) {
  glViewport(x, y, width, height);
}

FlurrOGLRenderer& FlurrOGLRenderer::Get() {
  static FlurrOGLRenderer flurr_renderer;
  return flurr_renderer;
}

Status FlurrOGLRenderer::OnInit() {
  // Initialize GLEW
  GLenum glew_result = glewInit();
  if (GLEW_OK != glew_result) {
    FLURR_LOG_ERROR("Failed to initialize GLEW (error %u)!", glew_result);
    return Status::kInitFailed;
  }
}

void FlurrOGLRenderer::OnShutdown() {
}

Status FlurrOGLRenderer::OnUpdate(float delta_time) {
  // Clear buffers
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  return Status::kSuccess;
}

ShaderProgram* FlurrOGLRenderer::OnCreateShaderProgram(ShaderProgramHandle program_handle) {
  return new OGLShaderProgram(program_handle);
}

} // namespace flurr
