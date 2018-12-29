#include "flurr/utils/ConfigFile.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/StringUtils.h"

#include <fstream>

namespace flurr
{

Status ConfigFile::readFromFile(const std::string& a_configPath)
{
  // Open config file for read
  std::ifstream ifs(a_configPath);
  if (!ifs.good())
  {
    FLURR_LOG_ERROR("Failed to open config file %s!", a_configPath.c_str());
    return Status::kOpenFileError;
  }

  // Read config file
  std::string line;
  m_lineNum = 0;
  m_curCategory = "";
  do
  {
    // Read a line from config file
    std::getline(ifs, line);
    if (ifs.bad())
    {
      FLURR_LOG_ERROR("Error reading config file %s!", a_configPath.c_str());
      return Status::kReadFileError;
    }

    if (!parseLine(line))
      FLURR_LOG_ERROR("Failed to parse line %u of config file %s!", m_lineNum, a_configPath.c_str());

    ++m_lineNum;
  }
  while (!ifs.eof());

  return Status::kSuccess;
}

Status ConfigFile::writeToFile(const std::string& a_configPath) const
{
  // Open config file for write
  std::ofstream ofs(a_configPath);
  if (!ofs.is_open())
  {
    FLURR_LOG_ERROR("Failed to open config file %s!", a_configPath.c_str());
    return Status::kOpenFileError;
  }

  // Write config file
  for (const auto& categoryKvp : m_valuesByCategory)
  {
    std::string category = categoryKvp.first;
    if ("" != category)
    {
      // Write category header
      ofs << "[" << category << "]" << std::endl;
    }

    for (const auto& kvp : categoryKvp.second)
    {
      // Write config setting
      ofs << kvp.first << "=" << kvp.second << std::endl;
    }
  }

  return Status::kSuccess;
}

bool ConfigFile::readBoolValue(const std::string& a_category, const std::string& a_key, bool& a_value) const
{
  return readValue<bool>(a_category, a_key, a_value);
}

void ConfigFile::writeBoolValue(const std::string& a_category, const std::string& a_key, bool a_value)
{
  writeValue<bool>(a_category, a_key, a_value);
}

bool ConfigFile::readFloatValue(const std::string& a_category, const std::string& a_key, float& a_value) const
{
  return readValue<float>(a_category, a_key, a_value);
}

void ConfigFile::writeFloatValue(const std::string& a_category, const std::string& a_key, float a_value)
{
  writeValue<float>(a_category, a_key, a_value);
}

bool ConfigFile::readIntValue(const std::string& a_category, const std::string& a_key, int& a_value) const
{
  return readValue<int>(a_category, a_key, a_value);
}

void ConfigFile::writeIntValue(const std::string& a_category, const std::string& a_key, int a_value)
{
  writeValue<int>(a_category, a_key, a_value);
}

bool ConfigFile::readStringValue(const std::string& a_category, const std::string& a_key, std::string& a_value) const
{
  return readValue<std::string>(a_category, a_key, a_value);
}

void ConfigFile::writeStringValue(const std::string& a_category, const std::string& a_key, const std::string& a_value)
{
  writeValue<std::string>(a_category, a_key, a_value);
}

bool ConfigFile::hasCategory(const std::string& a_category) const
{
  return m_valuesByCategory.count(a_category) > 0;
}

std::vector<std::string> ConfigFile::getCategories() const
{
  std::vector<std::string> categories;
  categories.reserve(m_valuesByCategory.size());
  for (const auto& categoryKvp : m_valuesByCategory)
    categories.push_back(categoryKvp.first);

  return categories;
}

bool ConfigFile::hasKey(const std::string& a_category, const std::string& a_key) const
{
  if (!hasCategory(a_category))
    return false;

  return m_valuesByCategory.find(a_category)->second.count(a_key) > 0;
}

std::vector<std::string> ConfigFile::getKeysInCategory(const std::string& a_category) const
{
  std::vector<std::string> keys;
  if (!hasCategory(a_category))
    return keys;

  auto categoryIt = m_valuesByCategory.find(a_category);
  keys.reserve(categoryIt->second.size());
  for (const auto& kvp : categoryIt->second)
    keys.push_back(kvp.first);

  return keys;
}

bool ConfigFile::parseLine(const std::string& a_line)
{
  std::string trimmedLine = TrimString(a_line);

  if ("" == trimmedLine)
  {
    // Empty line, skip
    return true;
  }
  else if ('[' == trimmedLine[0] &&
    ']' == trimmedLine[trimmedLine.length() - 1])
  {
    // Read category header
    std::string category = trimmedLine.substr(1, trimmedLine.length() - 2);
    if ("" == category)
    {
      FLURR_LOG_ERROR("Line %u: category name may not be an empty string!", m_lineNum);
      return false;
    }
    m_curCategory = category;
  }
  else
  {
    // Read setting key and value
    auto&& lineTokens = SplitString(trimmedLine, '=');
    if (lineTokens.size() < 2)
    {
      FLURR_LOG_ERROR("Line %u: invalid config setting!", m_lineNum);
      return false;
    }

    // Get key and value strings
    std::string key = TrimString(lineTokens[0]);
    std::string value = TrimString(
      trimmedLine.substr(trimmedLine.find_first_of('=') + 1));

    // Store setting key-value pair
    m_valuesByCategory[m_curCategory][key] = value;
  }

  return true;
}

template <typename T>
bool ConfigFile::readValue(const std::string& a_category, const std::string& a_key, T& a_value) const
{
  if (!hasKey(a_category, a_key))
    return false;

  a_value = FromString<T>(m_valuesByCategory.find(a_category)->second.find(a_key)->second);
  return true;
}

template <typename T>
void ConfigFile::writeValue(const std::string& a_category, const std::string& a_key, const T& a_value)
{
  m_valuesByCategory[a_category][a_key] = ToString<T>(a_value);
}

} // namespace flurr
