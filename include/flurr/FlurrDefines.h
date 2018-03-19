#pragma once

#include <stdint.h>

namespace flurr {

using Handle = uint32_t;

enum Status {
  kSuccess,
  kInitFailed,
  kShutdownFailed
};

enum RendererType {
  kOGL3
};

} // namespace flurr
