#pragma once
#include "FlurrApplication.h"

namespace flurr {

class HelloTriangleApplication : public FlurrApplication {

public:

  HelloTriangleApplication(int a_windowWidth, int a_windowHeight);

private:

  bool onInit() override;
  bool onUpdate(float a_deltaTime) override;
  void onDraw() override;
  void onQuit() override;

  static constexpr const char kVertexShaderPath[] = "resources/shaders/VertexColored.vert";
  static constexpr const char kFragmentShaderPath[] = "resources/shaders/VertexColored.frag";
  static constexpr const float kVertexData[] = {
    0.25f, 0.25f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f
  };
  static constexpr const float kColorData[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f
  };
  static constexpr const uint32_t kIndexData[] = {
    0, 1, 3,
    1, 2, 3
  };

  FlurrHandle m_shaderProgramHandle;
  FlurrHandle m_vb1Handle;
  FlurrHandle m_vb2Handle;
  FlurrHandle m_ibHandle;
  FlurrHandle m_vaHandle;
};

} // namespace flurr
