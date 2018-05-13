#pragma warning(disable:4996)
#include "FlurrLog.h"
#include "utils/FlurrTime.h"

#include <cassert>
#include <chrono>

namespace flurr {

FlurrLog::FlurrLog()
  : log_line_(0),
  file_write_line_(0),
  log_file_(fopen("flurr.log", "w")),
  file_write_thread_(&FlurrLog::WriteToFile, this) {
}

FlurrLog::~FlurrLog() {
  // Stop log file write thread
  if (file_write_thread_.joinable())
    file_write_thread_.join();

  // Close log file
  if (nullptr != log_file_) {
    fprintf(log_file_, "Log system has dropped %d messages.", num_dropped_msgs_);
    fclose(log_file_);
  }
}

#ifdef NDEBUG
bool FlurrLog::LogDebug(const char* file_path, int line, const char* msg, ...) {
  std::va_list args;
  va_start(args, msg);
  bool success = Log(file_path, line, "D", msg, args);
  va_end(args);

  return success;
}
#endif

bool FlurrLog::LogError(const char* file_path, int line, const char* msg, ...) {
  std::va_list args;
  va_start(args, msg);
  bool success = Log(file_path, line, "ERROR", msg, args);
  va_end(args);

  return success;
}

bool FlurrLog::LogWarning(const char* file_path, int line, const char* msg, ...) {
  std::va_list args;
  va_start(args, msg);
  bool success = Log(file_path, line, "WARNING", msg, args);
  va_end(args);

  return success;
}

bool FlurrLog::LogInfo(const char* file_path, int line, const char* msg, ...) {
  std::va_list args;
  va_start(args, msg);
  bool success = Log(file_path, line, "I", msg, args);
  va_end(args);

  return success;
}

void FlurrLog::Assert(const char* file_path, int line, bool result, const char* msg, ...) {
  if (!result) {
    std::va_list args;
    va_start(args, msg);
    bool success = Log(file_path, line, "ASSERTION FAILED", msg, args);
    va_end(args);

    assert(result);
  }
}

FlurrLog& FlurrLog::Get() {
  static FlurrLog log;
  return log;
}

bool FlurrLog::Log(const char* file_path, int line, const char* msg_type, const char* msg, va_list args) {
  assert(file_path != nullptr);

  if (!log_file_)
    return false;

  // Extract filename from path
  int last_slash_index = -1;
  for (int index = 0; file_path[index] != 0; ++index) {
    if (file_path[index] == '\\' || file_path[index] == '/')
      last_slash_index = index;
  }
  const char* filename = last_slash_index > -1 ? &file_path[last_slash_index + 1] : file_path;

  // Write log message to buffer
  {
    std::lock_guard<std::mutex> log_lock(log_mutex_);

    // Read current log line numbers
    int log_line = log_line_;
    int file_write_line = file_write_line_;

    // Write log line to buffer
    std::snprintf(
      msg_temp_,
      kMaxLineSize,
      "%s %s:%d %s %s\n",
      CurrentTimeAsStr().c_str(),
      filename,
      line,
      msg_type,
      msg);
    std::vsnprintf(msg_buffer_[log_line], kMaxLineSize, msg_temp_, args);

    // Increment log line number
    log_line = log_line < kMsgBufferSize - 1 ? log_line + 1 : 0;
    if (log_line == file_write_line) {
      // Buffer capacity exceeded, drop oldest message
      ++num_dropped_msgs_;
      file_write_line = file_write_line < kMsgBufferSize - 1 ?
        file_write_line + 1 : 0;
    }

    // Update log line line numbers
    log_line_ = log_line;
    file_write_line_ = file_write_line;
  }

  return true;
}

void FlurrLog::WriteToFile() {
  while (log_file_) {
    if (file_write_line_ == log_line_) {
      // Nothing to write, wait for more log messages
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    else {
      // Write buffered log messages to file
      {
        std::lock_guard<std::mutex> log_lock(log_mutex_);

        // Read current log line numbers
        int file_write_line = file_write_line_;
        int log_line = log_line_;

        // Write log lines to file
        while (file_write_line != log_line) {
          fprintf(log_file_, msg_buffer_[file_write_line]);

          // Increment file log line number
          file_write_line = file_write_line < kMsgBufferSize - 1 ?
            file_write_line + 1 : 0;
        }

        // Update file log line number
        file_write_line_ = file_write_line;
      }
      fflush(log_file_);
    }
  }
}

} // namespace flurr
