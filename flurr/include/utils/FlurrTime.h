#pragma once

#include "FlurrDefines.h"

#include <chrono>
#include <string>

namespace flurr {

inline auto StartTime() {
  static auto start_time = std::chrono::high_resolution_clock::now();
  return start_time;
}

/** Get time in milliseconds since first call to CurrentTime. */
inline constexpr double CurrentTime() {
  return std::chrono::duration_cast<std::chrono::duration<double>>(
    std::chrono::high_resolution_clock::now() - StartTime()).count();
}

/** Get current time as string formatted as MM:SS.ms. */
FLURR_DLL_EXPORT std::string CurrentTimeAsStr();

} // namespace flurr
