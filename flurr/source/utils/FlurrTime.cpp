#include "utils/FlurrTime.h"

#include <sstream>

namespace flurr {

std::string CurrentTimeAsStr() {
  // Compute current time in minutes, seconds and milliseconds
  long cur_time = static_cast<long>(CurrentTime() * 1000 + 0.5); // ms
  long cur_time_min = cur_time / 60000;
  long cur_time_sec = (cur_time - cur_time_min * 60000) / 1000;
  long cur_time_ms = cur_time - cur_time_min * 60000 - cur_time_sec * 1000;

  // Write time to string
  std::ostringstream oss;
  oss << cur_time_min << ":" << cur_time_sec << ":" << cur_time_sec << "." << cur_time_ms;

  return oss.str();
}

} // namespace flurr
