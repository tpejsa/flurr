#pragma once

#include "flurr/FlurrDefines.h"

#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <thread>

#ifdef FLURR_DEBUG
#define FLURR_LOG_DEBUG(msg, ...) \
  flurr::FlurrLog::Get().logDebug(__FILE__, __LINE__, msg FLURR_VA_OPT_COMMA __VA_ARGS__)
#else
#define FLURR_LOG_DEBUG(msg, ...)
#endif
#define FLURR_LOG_ERROR(msg, ...) \
  flurr::FlurrLog::Get().logError(__FILE__, __LINE__, msg FLURR_VA_OPT_COMMA __VA_ARGS__)
#define FLURR_LOG_WARN(msg, ...) \
  flurr::FlurrLog::Get().logWarning(__FILE__, __LINE__, msg FLURR_VA_OPT_COMMA __VA_ARGS__)
#define FLURR_LOG_INFO(msg, ...) \
  flurr::FlurrLog::Get().logInfo(__FILE__, __LINE__, msg FLURR_VA_OPT_COMMA __VA_ARGS__)
#define FLURR_ASSERT(result, msg, ...) \
  flurr::FlurrLog::Get().logAssert(__FILE__, __LINE__, (result), msg FLURR_VA_OPT_COMMA __VA_ARGS__)

namespace flurr
{

class FLURR_DLL_EXPORT FlurrLog
{

private:

  FlurrLog();
  FlurrLog(const FlurrLog&) = delete;
  FlurrLog(FlurrLog&&) = delete;
  FlurrLog& operator=(const FlurrLog&) = delete;
  FlurrLog& operator=(FlurrLog&&) = delete;
  ~FlurrLog();

public:

  #ifdef FLURR_DEBUG
  bool logDebug(const char* a_filePath, int a_line, const char* a_msg, ...);
  #endif
  bool logError(const char* a_filePath, int a_line, const char* a_msg, ...);
  bool logWarning(const char* a_filePath, int a_line, const char* a_msg, ...);
  bool logInfo(const char* a_filePath, int a_line, const char* a_msg, ...);
  void logAssert(const char* a_filePath, int a_line, bool a_result, const char* a_msg, ...);
  static FlurrLog& Get();

private:

  bool log(const char* a_filePath, int a_line, const char* a_msgType, const char* a_msg, va_list args);
  void writeToFile();

  static constexpr int kMaxLineSize = 4096;
  static constexpr int kMsgBufferSize = 1024;

  char m_msgBuffer[kMsgBufferSize][kMaxLineSize];
  char m_msgTemp[kMaxLineSize];
  std::atomic<int> m_logLine; // next line to write into buffer
  std::atomic<int> m_fileWriteLine; // next line to write to file
  FILE* m_logFile;
  int m_numDroppedMsgs;

  std::thread m_fileWriteThread;
  std::mutex m_logMutex;

};

} // namespace flurr
