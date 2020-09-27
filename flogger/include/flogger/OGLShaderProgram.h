#pragma once

#include "flurr/renderer/ShaderProgram.h"
#include "flogger/OGLRenderer.h"

#include <GL/glew.h>

namespace flurr
{

class FLURR_DLL_EXPORT OGLShaderProgram : public ShaderProgram
{

public:

  OGLShaderProgram(FlurrHandle a_programHandle);
  ~OGLShaderProgram() override;

  GLuint getOGLProgramId() const { return m_oglProgramId; }

  bool setFloatValue(const std::string& a_name, float a_value) override;
  bool setVec2Value(const std::string& a_name, const glm::vec2& a_value) override;
  bool setVec3Value(const std::string& a_name, const glm::vec3& a_value) override;
  bool setVec4Value(const std::string& a_name, const glm::vec4& a_value) override;
  bool setMat4Value(const std::string& a_name, const glm::mat4& a_value) override;
  bool setIntValue(const std::string& a_name, int a_value) override;
  bool setUIntValue(const std::string& a_name, uint32_t a_value) override;
  bool setBoolValue(const std::string& a_name, bool a_value) override;

private:

  Shader* onCreateShader(ShaderType a_shaderType, ShaderProgram* a_owningProgram) override;
  Status onLinkProgram() override;
  void onDestroyProgram() override;
  void onUseProgram() override;

  GLuint m_oglProgramId;
};

} // namespace flurr
