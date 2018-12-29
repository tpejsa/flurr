#pragma once

#include "flurr/FlurrDefines.h"

#include <sstream>
#include <string>
#include <vector>

namespace flurr
{

FLURR_DLL_EXPORT std::string TrimString(const std::string& a_str);
FLURR_DLL_EXPORT std::vector<std::string> SplitString(const std::string& a_str, char a_delim);

template <typename T>
T FromString(const std::string& a_str)
{
  std::istringstream iss(a_str);
  T value;
  iss >> value;

  return value;
}

template <>
inline std::string FromString(const std::string& a_str)
{
  return a_str;
}

template <typename T>
std::string ToString(const T& a_value)
{
  std::ostringstream oss;
  oss << a_value;

  return oss.str();
}

template <>
inline std::string ToString(const std::string& a_value)
{
  return a_value;
}

} // namespace flurr
