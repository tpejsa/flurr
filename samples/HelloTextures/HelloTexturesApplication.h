#pragma once
#include "FlurrApplication.h"

namespace flurr {

class HelloTexturesApplication : public FlurrApplication {

public:

  HelloTexturesApplication(int a_windowWidth, int a_windowHeight);

private:

  bool onInit() override;
  bool onUpdate(float a_deltaTime) override;
  void onDraw() override;
  void onQuit() override;

  // Shader paths
  static constexpr const char kVSPath[] = "resources/common/shaders/LitPhong.vert";
  static constexpr const char kFSPath[] = "resources/common/shaders/LitPhong.frag";

  // Texture paths
  static constexpr const char kTex1Path[] = "resources/HelloTextures/BrickWall1.png";
  static constexpr const char kTex2Path[] = "resources/HelloTextures/BrickWall2.dds";

  // Quad 1 definition
  static constexpr const float kPositionData1[] = {
    0.0f, 0.0f, 0.0f,
    0.25f, -0.75f, 0.0f,
    -0.75f, -0.75f, 0.0f,
    -0.75f, 0.25f, 0.0f
  };
  static constexpr const float kUV0Data1[] = {
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
  };
  static constexpr const uint32_t kIndexData1[] = {
    0, 1, 3,
    1, 2, 3
  };

  // Quad 2 definition
  static constexpr const float kPositionData2[] = {
    0.0f, 0.0f, 0.0f,
    0.25f, -0.75f, 0.0f,
    -0.75f, -0.75f, 0.0f,
    -0.75f, 0.25f, 0.0f
  };
  static constexpr const float kUV0Data2[] = {
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
  };
  static constexpr const uint32_t kIndexData2[] = {
    0, 1, 3,
    1, 2, 3
  };

  // Shaders
  FlurrHandle m_vsResourceHandle;
  FlurrHandle m_fsResourceHandle;
  FlurrHandle m_spHandle;

  // Textures
  FlurrHandle m_tex1ResourceHandle;
  FlurrHandle m_tex2ResourceHandle;

  // Geometry 1
  FlurrHandle m_vb1PosHandle;
  FlurrHandle m_vb1UV0Handle;
  FlurrHandle m_ib1Handle;
  FlurrHandle m_va1Handle;

  // Geometry 2
  FlurrHandle m_vb2PosHandle;
  FlurrHandle m_vb2UV0Handle;
  FlurrHandle m_ib2Handle;
  FlurrHandle m_va2Handle;
};

} // namespace flurr
