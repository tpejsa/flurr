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

  // Shader paths
  static constexpr const char kVS1Path[] = "resources/common/shaders/UnlitVertexColored.vert";
  static constexpr const char kFS1Path[] = "resources/common/shaders/UnlitVertexColored.frag";
  static constexpr const char kVS2Path[] = "resources/common/shaders/LitPhong.vert";
  static constexpr const char kFS2Path[] = "resources/common/shaders/LitPhong.frag";

  // Geometry 1 definition (two vertex-colored triangles)
  static constexpr const float kPositionData1[] = {
    0.0f, 0.0f, 0.0f,
    0.25f, -0.75f, 0.0f,
    -0.75f, -0.75f, 0.0f,
    -0.75f, 0.25f, 0.0f
  };
  static constexpr const float kColorData1[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f
  };
  static constexpr const uint32_t kIndexData1[] = {
    0, 1, 3,
    1, 2, 3
  };

  // Geometry 2 definition (one uniformly-colored triangle)
  static constexpr const float kPositionData2[] = {
    0.75f, 0.75f, 0.0f,
    0.75f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f
  };
  static constexpr const uint32_t kIndexData2[] = {
    0, 1, 2
  };

  // Shaders
  FlurrHandle m_vs1ResourceHandle;
  FlurrHandle m_fs1ResourceHandle;
  FlurrHandle m_sp1Handle;
  FlurrHandle m_vs2ResourceHandle;
  FlurrHandle m_fs2ResourceHandle;
  FlurrHandle m_sp2Handle;

  // Geometry 1
  FlurrHandle m_vb1PosHandle;
  FlurrHandle m_vb1ColorHandle;
  FlurrHandle m_ib1Handle;
  FlurrHandle m_va1Handle;

  // Geometry 2
  FlurrHandle m_vb2PosHandle;
  FlurrHandle m_ib2Handle;
  FlurrHandle m_va2Handle;
  float m_albedoTime;
};

} // namespace flurr
