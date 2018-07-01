#pragma once

#include "FlurrDefines.h"
#include "renderer/FlurrShader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace flurr {

enum class ShaderProgramState : uint8_t {
  kCompiling = 0,
  kLinked,
  kDeleting
};

class FLURR_DLL_EXPORT ShaderProgram {

public:

  ShaderProgram(ShaderProgramHandle program_handle);
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&&) = default;
  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&&) = default;
  virtual ~ShaderProgram() = default;

  inline ShaderProgramHandle GetProgramHandle() const { return program_handle_; }
  inline ShaderProgramState GetProgramState() const { return program_state_; }
  Status CompileShader(ShaderType shader_type, const std::string& shader_path, ShaderCreateMode create_mode = ShaderCreateMode::kFromSource);
  bool HasShader(ShaderType shader_type) const;
  Shader* GetShader(ShaderType shader_type) const;
  inline const std::vector<Shader*>& GetAllShaders() const { return shaders_; }
  virtual Status LinkProgram();
  virtual void DeleteProgram();

private:

  virtual Shader* OnCreateShader(ShaderType shader_type, ShaderProgram* owning_program) = 0;
  virtual Status OnLinkProgram() = 0;
  virtual void OnDeleteProgram() = 0;

  ShaderProgramHandle program_handle_;
  ShaderProgramState program_state_;
  std::unordered_map<ShaderType, std::unique_ptr<Shader>> shaders_by_type_;
  std::vector<Shader*> shaders_;
};

} // namespace flurr
