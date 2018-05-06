#pragma once

#include "FlurrDefines.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace flurr {

class FLURR_DLL_EXPORT ConfigFile {

public:

  Status ReadFromFile(const std::string& config_path);
  Status WriteToFile(const std::string& config_path) const;
  bool ReadBoolValue(const std::string& category, const std::string& key, bool& value) const;
  void WriteBoolValue(const std::string& category, const std::string& key, bool value);
  bool ReadFloatValue(const std::string& category, const std::string& key, float& value) const;
  void WriteFloatValue(const std::string& category, const std::string& key, float value);
  bool ReadIntValue(const std::string& category, const std::string& key, int& value) const;
  void WriteIntValue(const std::string& category, const std::string& key, int value);
  bool ReadStringValue(const std::string& category, const std::string& key, std::string& value) const;
  void WriteStringValue(const std::string& category, const std::string& key, const std::string& value);
  bool HasCategory(const std::string& category) const;
  std::vector<std::string> GetCategories() const;
  bool HasKey(const std::string& category, const std::string& key) const;
  std::vector<std::string> GetKeysInCategory(const std::string& category) const;

private:

  bool ParseLine(const std::string& line);
  template <typename T> bool ReadValue(const std::string& category, const std::string& key, T& value) const;
  template <typename T> void WriteValue(const std::string& category, const std::string& key, const T& value);

  uint32_t line_num_;
  std::string cur_category_;

  using KeyValueMap = std::unordered_map<std::string, std::string>;
  std::unordered_map<std::string, KeyValueMap> values_by_category_;
};

} // namespace flurr
