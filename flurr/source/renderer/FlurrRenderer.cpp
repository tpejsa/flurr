#include "renderer/FlurrRenderer.h"
#include "FlurrCore.h"
#include "FlurrConfigFile.h"
#include "FlurrLog.h"

namespace flurr {

FlurrRenderer::FlurrRenderer()
  : initialized_(false),
  next_shader_program_handle_(1) {
}

FlurrRenderer::~FlurrRenderer() {
  if (initialized_)
    Shutdown();
}

Status FlurrRenderer::Init(const std::string& config_path) {
  FLURR_LOG_INFO("Initializing flurr renderer...");

  if (!FlurrCore::Get().IsInitialized()) {
    FLURR_LOG_ERROR("Failed to initialize flurr renderer; flurr core not initialized!");
    return Status::kNotInitialized;
  }

  // Make sure this renderer is set in FlurrCore
  FlurrCore::Get().SetRenderer(this);

  // Initialize renderer
  Status result = OnInit();
  if (Status::kSuccess != result) {
    FLURR_LOG_ERROR("Failed to initialize flurr renderer!");
    return result;
  }

  FLURR_LOG_INFO("flurr renderer initialized.");
  initialized_ = true;
  return result;
}

void FlurrRenderer::Shutdown() {
  FLURR_LOG_INFO("Shutting down flurr renderer...");
  if (!initialized_) {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  OnShutdown();
  initialized_ = false;
  FLURR_LOG_INFO("flurr renderer shutdown complete.");
}

Status FlurrRenderer::Update(float delta_time) {
  if (!initialized_) {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  return OnUpdate(delta_time);
}

ShaderProgramHandle FlurrRenderer::CreateShaderProgram() {
  if (!initialized_) {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return INVALID_SHADER_PROGRAM;
  }

  // Create ShaderProgram instance
  ShaderProgramHandle program_handle = next_shader_program_handle_++;
  shader_programs_.emplace(program_handle, std::unique_ptr<ShaderProgram>(OnCreateShaderProgram(program_handle)));
  return program_handle;
}

bool FlurrRenderer::HasShaderProgram(ShaderProgramHandle program_handle) const {
  return shader_programs_.find(program_handle) != shader_programs_.end();
}

ShaderProgram* FlurrRenderer::GetShaderProgram(ShaderProgramHandle program_handle) const {
  auto&& shader_program_it = shader_programs_.find(program_handle);
  return shader_programs_.end() == shader_program_it ?
    nullptr : shader_program_it->second.get();
}

Status FlurrRenderer::CompileShader(ShaderProgramHandle program_handle,
  ShaderType shader_type, const std::string& shader_path, ShaderCreateMode create_mode) {
  if (!initialized_) {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

   // Get ShaderProgram object
  auto* shader_program = GetShaderProgram(program_handle);
  if (!shader_program) {
    FLURR_LOG_WARN("No ShaderProgram with handle %u!", program_handle);
    return Status::kNoObjectWithHandle;
  }

  // Compile shader
  return shader_program->CompileShader(shader_type, shader_path, create_mode);
}

Status FlurrRenderer::LinkShaderProgram(ShaderProgramHandle program_handle) {
  if (!initialized_) {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  // Get ShaderProgram object
  auto* shader_program = GetShaderProgram(program_handle);
  if (!shader_program) {
    FLURR_LOG_WARN("No ShaderProgram with handle %u!", program_handle);
    return Status::kNoObjectWithHandle;
  }

  return shader_program->LinkProgram();
}

void FlurrRenderer::DeleteShaderProgram(ShaderProgramHandle program_handle) {
  if (!initialized_) {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  // Get ShaderProgram object
  auto* shader_program = GetShaderProgram(program_handle);
  if (!shader_program) {
    FLURR_LOG_WARN("No ShaderProgram with handle %u!", program_handle);
    return;
  }

  shader_program->DeleteProgram();
}

Status FlurrRenderer::UseShaderProgram(ShaderProgramHandle program_handle) {
  auto* shader_program = GetShaderProgram(program_handle);
  if (!shader_program) {
    FLURR_LOG_ERROR("Trying to use non-existent shader program %u!", program_handle);
    return Status::kNoObjectWithHandle;
  }

  if (shader_program->GetProgramState() != ShaderProgramState::kLinked) {
    FLURR_LOG_ERROR("Unable to use shader program %u because it isn't linked!", program_handle);
    return Status::kInvalidState;
  }

  shader_program->UseProgram();
}

} // namespace flurr
