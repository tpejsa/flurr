#pragma once

#include "FlurrDefines.h"

#include <cstdarg>
#include <cstdio>

#ifdef NDEBUG
#define FLURR_LOG_DEBUG(msg, ...) \
  flurr::FlurrLog::Get().LogDebug(__FILE__, __LINE__, msg FLURR_VA_OPT_COMMA __VA_ARGS__)
#else
#define FLURR_LOG_DEBUG(msg, ...)
#endif
#define FLURR_LOG_ERROR(msg, ...) \
  flurr::FlurrLog::Get().LogError(__FILE__, __LINE__, msg FLURR_VA_OPT_COMMA __VA_ARGS__)
#define FLURR_LOG_WARN(msg, ...) \
  flurr::FlurrLog::Get().LogWarning(__FILE__, __LINE__, msg FLURR_VA_OPT_COMMA __VA_ARGS__)
#define FLURR_LOG_INFO(msg, ...) \
  flurr::FlurrLog::Get().LogInfo(__FILE__, __LINE__, msg FLURR_VA_OPT_COMMA __VA_ARGS__)
#define FLURR_ASSERT(result, msg, ...) \
  flurr::FlurrLog::Get().Assert(__FILE__, __LINE__, (result), msg FLURR_VA_OPT_COMMA __VA_ARGS__)

namespace flurr {

class FLURR_DLL_EXPORT FlurrLog {

private:

  FlurrLog();
  FlurrLog(const FlurrLog&) = delete;
  FlurrLog(FlurrLog&&) = delete;
  FlurrLog& operator=(const FlurrLog&) = delete;
  FlurrLog& operator=(FlurrLog&&) = delete;
  ~FlurrLog();

public:

  #ifdef NDEBUG
  bool LogDebug(const char* filename, int line, const char* msg, ...);
  #endif
  bool LogError(const char* filename, int line, const char* msg, ...);
  bool LogWarning(const char* filename, int line, const char* msg, ...);
  bool LogInfo(const char* filename, int line, const char* msg, ...);
  void Assert(const char* filename, int line, bool result, const char* msg, ...);
  static FlurrLog& Get();

private:

  bool Log(const char* filename, int line, const char* msg_type, const char* msg, va_list args);

  static constexpr int kMaxLineSize = 4096;
  std::FILE* log_file_;
  char log_line_[kMaxLineSize];
};

} // namespace flurr
