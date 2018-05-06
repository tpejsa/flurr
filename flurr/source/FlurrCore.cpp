#include "FlurrCore.h"
#include "FlurrLog.h"

namespace flurr {

Status FlurrCore::Init(const std::string& config_path) {
  FLURR_LOG_INFO("Initializing flurr...");

  FLURR_LOG_INFO("flurr initialized.");
  return Status::kSuccess;
}

void FlurrCore::Shutdown() {
  FLURR_LOG_INFO("Shutting down flurr...");

  FLURR_LOG_INFO("flurr shutdown complete.");
}

Status FlurrCore::Update() {
  return Status::kSuccess;
}

FlurrCore& FlurrCore::Get() {
  static FlurrCore flurr_core;
  return flurr_core;
}

} // namespace flurr
