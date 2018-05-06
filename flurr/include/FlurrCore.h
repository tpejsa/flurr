#pragma once

#include "FlurrDefines.h"
#include "FlurrConfigFile.h"

#include <string>

namespace flurr {

class FLURR_DLL_EXPORT FlurrCore {

private:

  FlurrCore() = default;
  FlurrCore(const FlurrCore&) = delete;
  FlurrCore(FlurrCore&&) = delete;
  FlurrCore& operator=(const FlurrCore&) = delete;
  FlurrCore& operator=(FlurrCore&&) = delete;
  ~FlurrCore() = default;

public:

  Status Init(const std::string& config_path = "flurr.cfg");
  void Shutdown();
  Status Update();

  static FlurrCore& Get();

private:

};

} // namespace flurr
