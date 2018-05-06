#include "utils/FlurrString.h"
#include "FlurrLog.h"

#include <cctype>
#include <sstream>

namespace flurr {

std::string TrimString(const std::string& str) {
  if (str.length() <= 0)
    return str;

  // Trim left side
  std::size_t left = 0;
  while (left < str.length() && std::isspace(str[left]))
    ++left;
  if (left >= str.length())
    return "";

  // Trim right side
  std::size_t right = str.size() - 1;
  while (right >= 0 && right < str.length() && std::isspace(str[right]))
    --right;
  FLURR_ASSERT(left <= right && right < str.length(),
    "Left index (%u) should not be higher than right index (%u)!",
    left, right);

  return str.substr(left, right - left + 1);
}

std::vector<std::string> SplitString(const std::string& str, char delim) {
  std::istringstream iss(str);
  std::string cur_token;
  std::vector<std::string> tokens;
  while (std::getline(iss, cur_token, delim)) {
    tokens.push_back(cur_token);
  }

  return tokens;
}

} // namespace flurr
