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

  bool setFloatValue(const std::string& name, float value) override;
  bool setVec2Value(const std::string& name, const glm::vec2& value) override;
  bool setVec3Value(const std::string& name, const glm::vec3& value) override;
  bool setVec4Value(const std::string& name, const glm::vec4& value) override;
  bool setIntValue(const std::string& name, int value) override;
  bool setUIntValue(const std::string& name, uint32_t value) override;
  bool setBoolValue(const std::string& name, bool value) override;

private:

  Shader* onCreateShader(ShaderType a_shaderType, ShaderProgram* a_owningProgram) override;
  Status onLinkProgram() override;
  void onDestroyProgram() override;
  void onUseProgram() override;

  GLuint m_oglProgramId;
};

} // namespace flurr
