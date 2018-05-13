#include "HelloTriangleApplication.h"

namespace flurr {

HelloTriangleApplication::HelloTriangleApplication(int window_width, int window_height)
  : FlurrApplication(window_width, window_height, "HelloTriangle") {
}

bool HelloTriangleApplication::OnInit() {
  return true;
}

bool HelloTriangleApplication::OnUpdate(float delta_time) {
  return true;
}

void HelloTriangleApplication::OnQuit() {
}

} // namespace flurr
