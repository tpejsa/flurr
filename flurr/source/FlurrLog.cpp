#pragma warning(disable:4996)
#include "FlurrLog.h"
#include "utils/FlurrTime.h"

#include <cassert>

namespace flurr {

FlurrLog::FlurrLog()
  : log_file_(fopen("flurr.log", "w")) {
}

FlurrLog::~FlurrLog() {
  if (nullptr != log_file_)
    fclose(log_file_);
}

#ifdef NDEBUG
bool FlurrLog::LogDebug(const char* filename, int line, const char* msg, ...) {
  std::va_list args;
  va_start(args, msg);
  bool success = Log(filename, line, "D", msg, args);
  va_end(args);

  return success;
}
#endif

bool FlurrLog::LogError(const char* filename, int line, const char* msg, ...) {
  std::va_list args;
  va_start(args, msg);
  bool success = Log(filename, line, "ERROR", msg, args);
  va_end(args);

  return success;
}

bool FlurrLog::LogWarning(const char* filename, int line, const char* msg, ...) {
  std::va_list args;
  va_start(args, msg);
  bool success = Log(filename, line, "WARNING", msg, args);
  va_end(args);

  return success;
}

bool FlurrLog::LogInfo(const char* filename, int line, const char* msg, ...) {
  std::va_list args;
  va_start(args, msg);
  bool success = Log(filename, line, "I", msg, args);
  va_end(args);

  return success;
}

void FlurrLog::Assert(const char* filename, int line, bool result, const char* msg, ...) {
  if (!result) {
    std::va_list args;
    va_start(args, msg);
    bool success = Log(filename, line, "ASSERTION FAILED", msg, args);
    va_end(args);

    assert(result);
  }
}

FlurrLog& FlurrLog::Get() {
  static FlurrLog log;
  return log;
}

bool FlurrLog::Log(const char* filename, int line, const char* msg_type, const char* msg, va_list args) {
  assert(filename != nullptr);

  if (!log_file_)
    return false;

  std::snprintf(
    log_line_,
    kMaxLineSize,
    "%s %s:%d %s %s\n",
    CurrentTimeAsStr().c_str(),
    filename,
    line,
    msg_type,
    msg);
  return std::vfprintf(log_file_, log_line_, args) >= 0;
}

} // namespace flurr
