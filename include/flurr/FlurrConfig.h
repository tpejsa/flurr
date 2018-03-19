#pragma once

#include "FlurrDefines.h"

#include <string>
#include <unordered_map>

namespace flurr {

class FlurrConfig {

public:

  FlurrConfig();
  ~FlurrConfig() = default;
  FlurrConfig(const FlurrCore&) = default;
  FlurrConfig(FlurrCore&&) = default;
  FlurrConfig& operator=(const FlurrCore&) = default;
  FlurrConfig& operator=(FlurrCore&&) = default;

  Status ReadFromFile(const std::string& config_path);
  Status WriteToFile(const std::string& config_path) const;
  bool ReadBoolValue(const std::string& category, const std::string& key) const;
  void WriteBoolValue(const std::string& category, const std::string& key, bool value);
  float ReadFloatValue(const std::string& category, const std::string& key) const;
  void WriteFloatValue(const std::string& category, const std::string& key, float value);
  int ReadIntValue(const std::string& category, const std::string& key) const;
  void WriteIntValue(const std::string& category, const std::string& key, int value);
  const std::string& ReadStringValue(const std::string& category, const std::string& key) const;
  void WriteStringValue(const std::string& category, const std::string& key, const std::string& value);

private:

  using KeyValueMap = std::unordered_map<std::string, std::string>;
  std::unordered_map<std::string, KeyValueMap> values_by_category_;
};

} // namespace flurr
