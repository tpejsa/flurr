#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/renderer/Shader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace flurr
{

enum class ShaderProgramState : uint8_t
{
  kDestroyed = 0,
  kCompiled,
  kLinked
};

class FLURR_DLL_EXPORT ShaderProgram
{
  friend class Renderer;

public:

  ShaderProgram(FlurrHandle program_handle);
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&&) = default;
  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&&) = default;
  virtual ~ShaderProgram() = default;

  FlurrHandle getProgramHandle() const { return m_programHandle; }
  ShaderProgramState getProgramState() const { return m_programState; }
  Status compileShader(ShaderType a_shaderType, FlurrHandle a_shaderResourceHandle);
  bool hasShader(ShaderType a_shaderType) const;
  Shader* getShader(ShaderType a_shaderType) const;
  const std::vector<Shader*>& getAllShaders() const { return m_shaders; }

  virtual bool setFloatValue(const std::string& a_name, float a_value) = 0;
  virtual bool setVec2Value(const std::string& a_name, const glm::vec2& a_value) = 0;
  virtual bool setVec3Value(const std::string& a_name, const glm::vec3& a_value) = 0;
  virtual bool setVec4Value(const std::string& a_name, const glm::vec4& a_value) = 0;
  virtual bool setMat4Value(const std::string& a_name, const glm::mat4& a_value) = 0;
  virtual bool setIntValue(const std::string& a_name, int a_value) = 0;
  virtual bool setUIntValue(const std::string& a_name, uint32_t a_value) = 0;
  virtual bool setBoolValue(const std::string& a_name, bool a_value) = 0;

private:

  Status linkProgram();
  void destroyProgram();
  Status useProgram();

  virtual Shader* onCreateShader(ShaderType a_shaderType, ShaderProgram* a_owningProgram) = 0;
  virtual Status onLinkProgram() = 0;
  virtual void onDestroyProgram() = 0;
  virtual void onUseProgram() = 0;

  FlurrHandle m_programHandle;
  ShaderProgramState m_programState;
  std::unordered_map<ShaderType, std::unique_ptr<Shader>> m_shadersByType;
  std::vector<Shader*> m_shaders;
};

} // namespace flurr
