#include "HelloTriangleApplication.h"

using flurr::HelloTriangleApplication;

int main(int argc, char* argv[]) {
  HelloTriangleApplication app(1024, 768);
  return app.Run();
}
