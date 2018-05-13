#pragma once
#include "FlurrApplication.h"

namespace flurr {

class HelloTriangleApplication : public FlurrApplication {

public:

  HelloTriangleApplication(int window_width, int window_height);

private:

  bool OnInit() override;
  bool OnUpdate(float delta_time) override;
  void OnQuit() override;
};

} // namespace flurr
