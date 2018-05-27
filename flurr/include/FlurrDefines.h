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

enum class Status : uint16_t {
  kSuccess = 0,
  kInitFailed,
  kOpenFileError,
  kReadFileError,
  kNotInitialized
};

enum class RendererType : uint8_t {
  kOpenGL
};

} // namespace flurr
