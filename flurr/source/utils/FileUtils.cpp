#include "flurr/utils/FileUtils.h"

#include <algorithm>

namespace flurr
{

std::string GetFileExtension(const std::string& a_path)
{
  if (a_path.size() < 3)
  {
    // Not a valid filename
    return "";
  }

  for (std::size_t char_index = a_path.size() - 1; char_index != 0; --char_index)
  {
    if (a_path[char_index] == '.')
    {
      std::string ext = a_path.substr(char_index + 1);
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      return ext;
    }
  }

  return "";
}

} // namespace flurr
