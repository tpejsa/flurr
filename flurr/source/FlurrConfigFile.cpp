#pragma once

#include "FlurrConfigFile.h"
#include "FlurrLog.h"
#include "utils/FlurrString.h"

#include <fstream>

namespace flurr {

Status ConfigFile::ReadFromFile(const std::string& config_path) {
  // Open config file for read
  std::ifstream ifs(config_path);
  if (!ifs.good()) {
    FLURR_LOG_ERROR("Failed to open config file %s!", config_path.c_str());
    return Status::kOpenFileError;
  }

  // Read config file
  std::string line;
  line_num_ = 0;
  cur_category_ = "";
  do {
    // Read a line from config file
    std::getline(ifs, line);
    if (ifs.bad()) {
      FLURR_LOG_ERROR("Error reading config file %s!", config_path.c_str());
      return Status::kReadFileError;
    }

    if (!ParseLine(line))
      FLURR_LOG_ERROR("Failed to parse line %u of config file %s!", line_num_, config_path.c_str());

    ++line_num_;
  } while (!ifs.eof());

  return Status::kSuccess;
}

Status ConfigFile::WriteToFile(const std::string& config_path) const {
  // Open config file for write
  std::ofstream ofs(config_path);
  if (!ofs.is_open()) {
    FLURR_LOG_ERROR("Failed to open config file %s!", config_path.c_str());
    return Status::kOpenFileError;
  }

  // Write config file
  for (const auto& category_kvp : values_by_category_) {
    std::string category = category_kvp.first;
    if ("" != category) {
      // Write category header
      ofs << "[" << category << "]" << std::endl;
    }

    for (const auto& kvp : category_kvp.second) {
      // Write config setting
      ofs << kvp.first << "=" << kvp.second << std::endl;
    }
  }

  return Status::kSuccess;
}

bool ConfigFile::ReadBoolValue(const std::string& category, const std::string& key, bool& value) const {
  return ReadValue<bool>(category, key, value);
}

void ConfigFile::WriteBoolValue(const std::string& category, const std::string& key, bool value) {
  WriteValue<bool>(category, key, value);
}

bool ConfigFile::ReadFloatValue(const std::string& category, const std::string& key, float& value) const {
  return ReadValue<float>(category, key, value);
}

void ConfigFile::WriteFloatValue(const std::string& category, const std::string& key, float value) {
  WriteValue<float>(category, key, value);
}

bool ConfigFile::ReadIntValue(const std::string& category, const std::string& key, int& value) const {
  return ReadValue<int>(category, key, value);
}

void ConfigFile::WriteIntValue(const std::string& category, const std::string& key, int value) {
  WriteValue<int>(category, key, value);
}

bool ConfigFile::ReadStringValue(const std::string& category, const std::string& key, std::string& value) const {
  return ReadValue<std::string>(category, key, value);
}

void ConfigFile::WriteStringValue(const std::string& category, const std::string& key, const std::string& value) {
  WriteValue<std::string>(category, key, value);
}

bool ConfigFile::HasCategory(const std::string& category) const {
  return values_by_category_.count(category) > 0;
}

std::vector<std::string> ConfigFile::GetCategories() const {
  std::vector<std::string> categories;
  categories.reserve(values_by_category_.size());
  for (const auto& category_kvp : values_by_category_)
    categories.push_back(category_kvp.first);

  return categories;
}

bool ConfigFile::HasKey(const std::string& category, const std::string& key) const {
  if (!HasCategory(category))
    return false;

  return values_by_category_.find(category)->second.count(key) > 0;
}

std::vector<std::string> ConfigFile::GetKeysInCategory(const std::string& category) const {
  std::vector<std::string> keys;
  if (!HasCategory(category))
    return keys;

  auto category_it = values_by_category_.find(category);
  keys.reserve(category_it->second.size());
  for (const auto& kvp : category_it->second)
    keys.push_back(kvp.first);

  return keys;
}

bool ConfigFile::ParseLine(const std::string& line) {
  std::string trimmed_line = TrimString(line);

  if ("" == trimmed_line) {
    // Empty line, skip
    return true;
  }
  else if ('[' == trimmed_line[0] &&
    ']' == trimmed_line[trimmed_line.length() - 1]) {
    // Read category header
    std::string category = trimmed_line.substr(1, trimmed_line.length() - 2);
    if ("" == category) {
      FLURR_LOG_ERROR("Line %u: category name may not be an empty string!", line_num_);
      return false;
    }
    cur_category_ = category;
  } else {
    // Read setting key and value
    auto&& line_tokens = SplitString(trimmed_line, '=');
    if (line_tokens.size() < 2) {
      FLURR_LOG_ERROR("Line %u: invalid config setting!", line_num_);
      return false;
    }

    // Get key and value strings
    std::string key = TrimString(line_tokens[0]);
    std::string value = TrimString(
      trimmed_line.substr(trimmed_line.find_first_of('=') + 1));

    // Store setting key-value pair
    values_by_category_[cur_category_][key] = value;
  }

  return true;
}

template <typename T>
bool ConfigFile::ReadValue(const std::string& category, const std::string& key, T& value) const {
  if (!HasKey(category, key))
    return false;

  value = FromString<T>(values_by_category_.find(category)->second.find(key)->second);
  return true;
}

template <typename T>
void ConfigFile::WriteValue(const std::string& category, const std::string& key, const T& value) {
  values_by_category_[category][key] = ToString<T>(value);
}

} // namespace flurr
