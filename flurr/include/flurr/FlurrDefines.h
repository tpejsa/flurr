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
constexpr FlurrHandle INVALID_HANDLE = 0;

// Generate an object handle given the next expected available object handle
// and a function to check handle availability
template <typename F>
FlurrHandle GenerateHandle(FlurrHandle& a_nh, const F& a_hasObj)
{
  while (INVALID_HANDLE != a_nh && a_hasObj(a_nh) || INVALID_HANDLE == a_nh)
    ++a_nh;

  return a_nh;
}

// flurr status codes
enum class Status : uint16_t
{
  kSuccess = 0,
  kFailed,
  kNotImplemented,
  kNullArgument,
  kInvalidArgument,
  kIndexOutOfBounds,
  kOpenFileError,
  kReadFileError,
  kUnsupportedFileType,
  kNotInitialized,
  kInvalidHandle,
  kUnsupportedType,
  kUnsupportedMode,
  kCompilationFailed,
  kLinkingFailed,
  kInvalidState,
  kResourceFileNotFound,
  kResourceAlreadyExists,
  kResourceNotCreated,
  kResourceAlreadyLoaded,
  kResourceNotLoaded,
  kResourceDestroying,
  kResourceTypeInvalid,
  kUnsupportedTextureFormat,
  kCount
};

// Priorities
using Priority = uint32_t;
constexpr Priority MAX_PRIORITY = 0;
constexpr Priority VERY_HIGH_PRIORITY = 10;
constexpr Priority HIGH_PRIORITY = 20;
constexpr Priority NORMAL_PRIORITY = 30;
constexpr Priority LOW_PRIORITY = 40;
constexpr Priority VERY_LOW_PRIORITY = 40;
constexpr Priority MIN_PRIORITY = 50;

// Shader uniforms
constexpr char* const MODEL_TRANSFORM_UNIFORM_NAME = "modelTransf";
constexpr char* const VIEW_PROJECTION_TRANSFORM_UNIFORM_NAME = "viewProjTransf";

} // namespace flurr
