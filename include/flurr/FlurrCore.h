#pragma once

#include "FlurrDefines.h"
#include "FlurrConfig.h"

#include <string>

namespace flurr {

class FlurrCore {

public:

  FlurrCore() = default;
  ~FlurrCore() = default;
  FlurrCore(const FlurrCore&) = delete;
  FlurrCore(FlurrCore&&) = delete;
  FlurrCore& operator=(const FlurrCore&) = delete;
  FlurrCore& operator=(FlurrCore&&) = delete;

  Status Init(const std::string& config_path = "flurr.cfg");
  Status Shutdown();
  Status Update();

private:

  FlurrConfig config_;
};

} // namespace flurr
