#pragma once

#include "FlurrDefines.h"

#include <string>

namespace flurr {

class ShaderProgram;

enum class ShaderType : uint8_t {
  kVertex = 0,
  kGeometry,
  kFragment
};

enum class ShaderCreateMode : uint8_t {
  kFromSource = 0,
  kFromBinary
};

class FLURR_DLL_EXPORT Shader {

public:

  Shader(ShaderType shader_type, ShaderProgram* owning_program);
  Shader(const Shader&) = default;
  Shader(Shader&&) = default;
  Shader& operator=(const Shader&) = default;
  Shader& operator=(Shader&&) = default;
  virtual ~Shader() = default;

  inline ShaderType GetShaderType() const { return shader_type_; }
  inline ShaderProgram* GetOwningProgram() const { return owning_program_; }
  virtual Status Compile(const std::string& shader_path, ShaderCreateMode create_mode) = 0;
  virtual void Delete() = 0;

private:

  ShaderType shader_type_;
  ShaderProgram* owning_program_;
};

} // namespace flurr
