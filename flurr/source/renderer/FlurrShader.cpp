#include "renderer/FlurrShader.h"
#include "renderer/FlurrShaderProgram.h"

namespace flurr {

Shader::Shader(ShaderType shader_type, ShaderProgram* owning_program)
  : shader_type_(shader_type),
  owning_program_(owning_program) {
}

} // namespace flurr
