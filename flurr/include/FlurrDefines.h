#pragma once

#include <stdint.h>

// Shared library export
#ifdef _MSC_VER
  #define FLURR_DLL_EXPORT __declspec(dllexport)
#else
#define FLURR_DLL_EXPORT
#endif

// Variadic macro support
#ifdef _MSC_VER
  #define FLURR_VA_OPT_COMMA ,
#else
  #define FLURR_VA_OPT_COMMA __VA_OPT__(,)
#endif

namespace flurr {

using Handle = uint32_t;

enum Status {
  kSuccess,
  kInitFailed,
  kShutdownFailed,
  kOpenFileError,
  kReadFileError
};

enum RendererType {
  kOGL3
};

} // namespace flurr
