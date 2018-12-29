#include "flurr/utils/StringUtils.h"
#include "flurr/FlurrLog.h"

#include <cctype>
#include <sstream>

namespace flurr
{

std::string TrimString(const std::string& a_str)
{
  if (a_str.length() <= 0)
    return a_str;

  // Trim left side
  std::size_t left = 0;
  while (left < a_str.length() && std::isspace(a_str[left]))
    ++left;
  if (left >= a_str.length())
    return "";

  // Trim right side
  std::size_t right = a_str.size() - 1;
  while (right >= 0 && right < a_str.length() && std::isspace(a_str[right]))
    --right;
  FLURR_ASSERT(left <= right && right < a_str.length(),
    "Left index (%u) should not be higher than right index (%u)!",
    left, right);

  return a_str.substr(left, right - left + 1);
}

std::vector<std::string> SplitString(const std::string& a_str, char a_delim)
{
  std::istringstream iss(a_str);
  std::string curToken;
  std::vector<std::string> tokens;
  while (std::getline(iss, curToken, a_delim))
  {
    tokens.push_back(curToken);
  }

  return tokens;
}

} // namespace flurr
