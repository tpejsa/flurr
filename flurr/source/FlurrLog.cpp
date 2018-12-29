#pragma warning(disable:4996)
#include "flurr/FlurrLog.h"
#include "flurr/utils/TimeUtils.h"

#include <cassert>
#include <chrono>

namespace flurr
{

FlurrLog::FlurrLog()
  : m_logLine(0),
  m_fileWriteLine(0),
  m_logFile(fopen("flurr.log", "w")),
  m_fileWriteThread(&FlurrLog::writeToFile, this)
{
}

FlurrLog::~FlurrLog()
{
  // Stop log file write thread
  if (m_fileWriteThread.joinable())
    m_fileWriteThread.join();

  // Close log file
  if (nullptr != m_logFile)
  {
    fprintf(m_logFile, "Log system has dropped %d messages.", m_numDroppedMsgs);
    fclose(m_logFile);
  }
}

#ifdef FLURR_DEBUG
bool FlurrLog::logDebug(const char* a_filePath, int a_line, const char* a_msg, ...)
{
  std::va_list args;
  va_start(args, a_msg);
  bool success = log(a_filePath, a_line, "", a_msg, args);
  va_end(args);

  return success;
}
#endif

bool FlurrLog::logError(const char* a_filePath, int a_line, const char* a_msg, ...)
{
  std::va_list args;
  va_start(args, a_msg);
  bool success = log(a_filePath, a_line, "ERROR ", a_msg, args);
  va_end(args);

  return success;
}

bool FlurrLog::logWarning(const char* a_filePath, int a_line, const char* a_msg, ...)
{
  std::va_list args;
  va_start(args, a_msg);
  bool success = log(a_filePath, a_line, "WARNING ", a_msg, args);
  va_end(args);

  return success;
}

bool FlurrLog::logInfo(const char* a_filePath, int a_line, const char* a_msg, ...)
{
  std::va_list args;
  va_start(args, a_msg);
  bool success = log(a_filePath, a_line, "", a_msg, args);
  va_end(args);

  return success;
}

void FlurrLog::logAssert(const char* a_filePath, int a_line, bool a_result, const char* a_msg, ...)
{
  if (!a_result)
  {
    std::va_list args;
    va_start(args, a_msg);
    bool success = log(a_filePath, a_line, "ASSERTION FAILED", a_msg, args);
    va_end(args);

    assert(a_result);
  }
}

FlurrLog& FlurrLog::Get()
{
  static FlurrLog log;
  return log;
}

bool FlurrLog::log(const char* a_filePath, int a_line, const char* a_msgType, const char* a_msg, va_list args)
{
  assert(a_filePath != nullptr);

  if (!m_logFile)
    return false;

  // Extract filename from path
  int lastSlashIndex = -1;
  for (int index = 0; a_filePath[index] != 0; ++index)
  {
    if (a_filePath[index] == '\\' || a_filePath[index] == '/')
      lastSlashIndex = index;
  }
  const char* filename = lastSlashIndex > -1 ? &a_filePath[lastSlashIndex + 1] : a_filePath;

  // Write log message to buffer
  {
    std::lock_guard<std::mutex> logLock(m_logMutex);

    // Read current log line numbers
    int logLine = m_logLine;
    int fileWriteLine = m_fileWriteLine;

    // Write log line to buffer
    std::snprintf(
      m_msgTemp,
      kMaxLineSize,
      "%s %s:%d %s%s\n",
      CurrentTimeAsStr().c_str(),
      filename,
      a_line,
      a_msgType,
      a_msg);
    std::vsnprintf(m_msgBuffer[logLine], kMaxLineSize, m_msgTemp, args);

    // Increment log line number
    logLine = logLine < kMsgBufferSize - 1 ? logLine + 1 : 0;
    if (logLine == fileWriteLine)
    {
      // Buffer capacity exceeded, drop oldest message
      ++m_numDroppedMsgs;
      fileWriteLine = fileWriteLine < kMsgBufferSize - 1 ?
        fileWriteLine + 1 : 0;
    }

    // update log line line numbers
    m_logLine = logLine;
    m_fileWriteLine = fileWriteLine;
  }

  return true;
}

void FlurrLog::writeToFile()
{
  while (m_logFile)
  {
    if (m_fileWriteLine == m_logLine)
    {
      // Nothing to write, wait for more log messages
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    else
    {
      // Write buffered log messages to file
      {
        std::lock_guard<std::mutex> log_lock(m_logMutex);

        // Read current log line numbers
        int fileWriteLine = m_fileWriteLine;
        int logLine = m_logLine;

        // Write log lines to file
        while (fileWriteLine != logLine)
        {
          fprintf(m_logFile, m_msgBuffer[fileWriteLine]);

          // Increment file log line number
          fileWriteLine = fileWriteLine < kMsgBufferSize - 1 ?
            fileWriteLine + 1 : 0;
        }

        // update file log line number
        m_fileWriteLine = fileWriteLine;
      }
      fflush(m_logFile);
    }
  }
}

} // namespace flurr
