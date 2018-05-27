#include "FlurrOGLRenderer.h"

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
  return Status::kSuccess;
}

} // namespace flurr
