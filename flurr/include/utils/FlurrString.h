#pragma once

#include "FlurrDefines.h"

#include <sstream>
#include <string>
#include <vector>

namespace flurr {

FLURR_DLL_EXPORT std::string TrimString(const std::string& str);
FLURR_DLL_EXPORT std::vector<std::string> SplitString(const std::string& str, char delim);

template <typename T>
T FromString(const std::string& str) {
  std::istringstream iss(str);
  T value;
  iss >> value;

  return value;
}

template <>
inline std::string FromString(const std::string& str) {
  return str;
}

template <typename T>
std::string ToString(const T& value) {
  std::ostringstream oss;
  oss << value;

  return oss.str();
}

template <>
inline std::string ToString(const std::string& value) {
  return value;
}

} // namespace flurr
