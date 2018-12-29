#include "flurr/utils/TimeUtils.h"

#include <chrono>
#include <sstream>

namespace flurr
{

double CurrentTime()
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::duration<double>>(
    std::chrono::high_resolution_clock::now() - startTime).count();
}

std::string CurrentTimeAsStr()
{
  // Compute current time in minutes, seconds and milliseconds
  long curTime = static_cast<long>(CurrentTime() * 1000 + 0.5); // ms
  long curTimeMin = curTime / 60000;
  long curTimeSec = (curTime - curTimeMin * 60000) / 1000;
  long curTimeMs = curTime - curTimeMin * 60000 - curTimeSec * 1000;

  // Write time to string
  std::ostringstream oss;
  oss << curTimeMin << ":" << curTimeSec << ":" << curTimeSec << "." << curTimeMs;

  return oss.str();
}

} // namespace flurr
