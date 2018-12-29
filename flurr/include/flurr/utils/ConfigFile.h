#pragma once

#include "flurr/FlurrDefines.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace flurr
{

class FLURR_DLL_EXPORT ConfigFile
{

public:

  Status readFromFile(const std::string& a_configPath);
  Status writeToFile(const std::string& a_configPath) const;
  bool readBoolValue(const std::string& a_category, const std::string& a_key, bool& a_value) const;
  void writeBoolValue(const std::string& a_category, const std::string& a_key, bool a_value);
  bool readFloatValue(const std::string& a_category, const std::string& a_key, float& a_value) const;
  void writeFloatValue(const std::string& a_category, const std::string& a_key, float a_value);
  bool readIntValue(const std::string& a_category, const std::string& a_key, int& a_value) const;
  void writeIntValue(const std::string& a_category, const std::string& a_key, int a_value);
  bool readStringValue(const std::string& a_category, const std::string& a_key, std::string& a_value) const;
  void writeStringValue(const std::string& a_category, const std::string& a_key, const std::string& a_value);
  bool hasCategory(const std::string& a_category) const;
  std::vector<std::string> getCategories() const;
  bool hasKey(const std::string& a_category, const std::string& a_key) const;
  std::vector<std::string> getKeysInCategory(const std::string& a_category) const;

private:

  bool parseLine(const std::string& a_line);
  template <typename T> bool readValue(const std::string& a_category, const std::string& a_key, T& a_value) const;
  template <typename T> void writeValue(const std::string& a_category, const std::string& a_key, const T& a_value);

  uint32_t m_lineNum;
  std::string m_curCategory;

  using KeyValueMap = std::unordered_map<std::string, std::string>;
  std::unordered_map<std::string, KeyValueMap> m_valuesByCategory;
};

} // namespace flurr
