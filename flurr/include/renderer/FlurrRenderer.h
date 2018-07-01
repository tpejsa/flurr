#pragma once

#include "FlurrDefines.h"
#include "renderer/FlurrShaderProgram.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace flurr {

class FLURR_DLL_EXPORT FlurrRenderer {

protected:

  FlurrRenderer();
  FlurrRenderer(const FlurrRenderer&) = delete;
  FlurrRenderer(FlurrRenderer&&) = delete;
  FlurrRenderer& operator=(const FlurrRenderer&) = delete;
  FlurrRenderer& operator=(FlurrRenderer&&) = delete;
  virtual ~FlurrRenderer();

public:

  Status Init(const std::string& config_path = "flurr_renderer.cfg");
  void Shutdown();
  Status Update(float delta_time);
  inline bool IsInitialized() const { return initialized_; }

  virtual void SetViewport(int x, int y, uint32_t width, uint32_t height) = 0;

  ShaderProgramHandle CreateShaderProgram();
  bool HasShaderProgram(ShaderProgramHandle program_handle) const;
  ShaderProgram* GetShaderProgram(ShaderProgramHandle program_handle) const;
  Status CompileShader(ShaderProgramHandle program_handle,
    ShaderType shader_type, const std::string& shader_path,
    ShaderCreateMode create_mode = ShaderCreateMode::kFromSource);
  Status LinkShaderProgram(ShaderProgramHandle program_handle);
  void DeleteShaderProgram(ShaderProgramHandle program_handle);
  virtual Status UseShaderProgram(ShaderProgramHandle program_handle);

private:

  virtual Status OnInit() = 0;
  virtual void OnShutdown() = 0;
  virtual Status OnUpdate(float delta_time) = 0;

  virtual ShaderProgram* OnCreateShaderProgram(ShaderProgramHandle program_handle) = 0;
  virtual Status OnUseShaderProgram(ShaderProgram* program) = 0;

  bool initialized_;

  // Shaders
  ShaderProgramHandle next_shader_program_handle_;
  std::unordered_map<ShaderProgramHandle, std::unique_ptr<ShaderProgram>> shader_programs_;
};

} // namespace flurr
