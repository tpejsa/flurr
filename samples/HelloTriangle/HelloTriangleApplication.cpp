#include "HelloTriangleApplication.h"

namespace flurr {

HelloTriangleApplication::HelloTriangleApplication(int window_width, int window_height)
  : FlurrApplication(window_width, window_height, "HelloTriangle"),
  shader_program_handle_(INVALID_SHADER_PROGRAM) {
}

bool HelloTriangleApplication::OnInit() {
  // Compile vertex color shaders
  auto* renderer = FlurrCore::Get().GetRenderer();
  shader_program_handle_ = renderer->CreateShaderProgram();
  if (Status::kSuccess != renderer->CompileShader(shader_program_handle_, ShaderType::kVertex, kVertexShaderPath)) {
    FLURR_LOG_ERROR("Failed to compile vertex shader %s!", kVertexShaderPath);
    return false;
  }
  if (Status::kSuccess != renderer->CompileShader(shader_program_handle_, ShaderType::kFragment, kFragmentShaderPath)) {
    FLURR_LOG_ERROR("Failed to compile fragment shader %s!", kFragmentShaderPath);
    return false;
  }

  // Link vertex color shaders
  if (Status::kSuccess != renderer->LinkShaderProgram(shader_program_handle_)) {
    FLURR_LOG_ERROR("Failed to link shader program!");
    return false;
  }

  return true;
}

bool HelloTriangleApplication::OnUpdate(float delta_time) {
  auto* renderer = FlurrCore::Get().GetRenderer();
  renderer->UseShaderProgram(shader_program_handle_);

  return true;
}

void HelloTriangleApplication::OnQuit() {
}

} // namespace flurr
