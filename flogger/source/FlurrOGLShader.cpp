#include "FlurrOGLShader.h"
#include "FlurrLog.h"
#include "utils/FlurrTypeCasts.h"

#include <fstream>
#include <streambuf>
#include <string>

namespace flurr {

OGLShader::OGLShader(ShaderType shader_type, ShaderProgram* owning_program)
  : Shader(shader_type, owning_program),
  ogl_shader_id_(0) {
}

OGLShader::~OGLShader() {
  if (ogl_shader_id_)
    glDeleteShader(ogl_shader_id_);
}

Status OGLShader::Compile(const std::string& shader_path, ShaderCreateMode create_mode) {
  GLenum ogl_shader_type = GetOGLShaderType(GetShaderType());
  if (0 == ogl_shader_type) {
    FLURR_LOG_ERROR("Unsupported shader type %u!", FromEnum(GetShaderType()));
    return Status::kUnsupportedType;
  }

  if (ShaderCreateMode::kFromSource != create_mode) {
    FLURR_LOG_ERROR("Shader can only be compiled from source!");
    return Status::kUnsupportedMode;
  }

  // Load shader source
  std::ifstream ifs(shader_path);
  if (!ifs.good()) {
    FLURR_LOG_ERROR("Failed to read shader source %s!", shader_path.c_str());
    return Status::kOpenFileError;
  }
  std::string shader_source_str((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  const auto* shader_source_data = shader_source_str.data();
  GLint shader_source_length = static_cast<GLint>(shader_source_str.length());

  // Create and compile shader
  ogl_shader_id_ = glCreateShader(ogl_shader_type);
  if (0 == ogl_shader_id_) {
    FLURR_LOG_ERROR("Failed to create shader!");
    return Status::kFailed;
  }
  glShaderSource(ogl_shader_id_, 1, &shader_source_data, &shader_source_length);
  glCompileShader(ogl_shader_id_);

  // Check for compilation errors
  GLint result = 0;
  glGetShaderiv(ogl_shader_id_, GL_COMPILE_STATUS, &result);
  if (!result) {
    static const int kInfoLogSize = 1024;
    GLchar info_log[kInfoLogSize];
    glGetShaderInfoLog(ogl_shader_id_, kInfoLogSize, nullptr, &info_log[0]);
    FLURR_LOG_ERROR("Failed to compile shader %s!\n%s", shader_path.c_str(), info_log);

    return Status::kCompilationFailed;
  }

  return Status::kSuccess;
}

void OGLShader::Delete() {
  if (ogl_shader_id_)
    glDeleteShader(ogl_shader_id_);
}

GLenum OGLShader::GetOGLShaderType(ShaderType shader_type) const {
  switch (shader_type) {
    case ShaderType::kVertex: {
      return GL_VERTEX_SHADER;
    }
    case ShaderType::kGeometry: {
      return GL_GEOMETRY_SHADER;
    }
    case ShaderType::kFragment: {
      return GL_FRAGMENT_SHADER;
    }
    default: {
      return 0;
    }
  }
}

} // namespace flurr
