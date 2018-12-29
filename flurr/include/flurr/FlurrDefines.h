#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

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

namespace flurr
{

// flurr object handles
using FlurrHandle = uint32_t;
constexpr FlurrHandle INVALID_OBJECT = 0;

// flurr status codes
enum class Status : uint16_t
{
  kSuccess = 0,
  kFailed,
  kNullArgument,
  kInvalidArgument,
  kOpenFileError,
  kReadFileError,
  kNotInitialized,
  kInvalidHandle,
  kUnsupportedType,
  kUnsupportedMode,
  kCompilationFailed,
  kLinkingFailed,
  kInvalidState
};

} // namespace flurr
