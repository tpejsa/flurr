#include "renderer/FlurrShaderProgram.h"
#include "FlurrLog.h"
#include "utils/FlurrTypeCasts.h"

namespace flurr {

ShaderProgram::ShaderProgram(ShaderProgramHandle program_handle)
  : program_handle_(program_handle),
  program_state_(ShaderProgramState::kCompiling) {
}

Status ShaderProgram::CompileShader(ShaderType shader_type, const std::string& shader_path, ShaderCreateMode create_mode) {
  if (ShaderProgramState::kCompiling != program_state_) {
    FLURR_LOG_ERROR("Unable to compile shader, program already linked!");
    return Status::kInvalidState;
  }

  if (HasShader(shader_type)) {
    FLURR_LOG_WARN("Compiling shader of type %u, which already exists in the current program!", FromEnum(shader_type));
    shaders_by_type_.at(shader_type)->Delete();
  }

  auto* shader = OnCreateShader(shader_type, this);
  Status compile_status = shader->Compile(shader_path, create_mode);
  if (Status::kSuccess != compile_status) {
    shader->Delete();
    delete shader;
    return compile_status;
  }
  shaders_by_type_.emplace(shader_type, std::unique_ptr<Shader>(shader));
  shaders_.push_back(shader);

  return Status::kSuccess;
}

bool ShaderProgram::HasShader(ShaderType shader_type) const {
  return shaders_by_type_.find(shader_type) != shaders_by_type_.end();
}

Shader* ShaderProgram::GetShader(ShaderType shader_type) const {
  auto&& shader_it = shaders_by_type_.find(shader_type);
  return shaders_by_type_.end() == shader_it ? nullptr : shader_it->second.get();
}

Status ShaderProgram::LinkProgram() {
  if (GetProgramState() != ShaderProgramState::kCompiling) {
    FLURR_LOG_ERROR("Shader program not in compiling state!");
    return Status::kInvalidState;
  }

  Status link_status = OnLinkProgram();
  if (Status::kSuccess == link_status)
    program_state_ = ShaderProgramState::kLinked;
  else
    DeleteProgram();
  return link_status;
}

void ShaderProgram::DeleteProgram() {
  OnDeleteProgram();
  program_state_ = ShaderProgramState::kDeleting;
}

void ShaderProgram::UseProgram() {
  OnUseProgram();
}

} // namespace flurr
