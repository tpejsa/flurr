#pragma once

#include "flurr.h"
#include <GL/glew.h>

#include <string>

namespace flurr {

class FLURR_DLL_EXPORT FlurrOGLRenderer : public FlurrRenderer {

private:

  FlurrOGLRenderer();
  ~FlurrOGLRenderer();

public:

  void SetViewport(int x, int y, uint32_t width, uint32_t height) override;

  static FlurrOGLRenderer& Get();

private:

  Status OnInit() override;
  void OnShutdown() override;
  Status OnUpdate(float delta_time) override;
};

} // namespace flurr
