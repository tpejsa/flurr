#include "FlurrCore.h"
#include "FlurrLog.h"

namespace flurr {

FlurrCore::FlurrCore()
  : initialized_(false) {
}

FlurrCore::~FlurrCore() {
  if (initialized_)
    Shutdown();
}

Status FlurrCore::Init(const std::string& config_path) {
  FLURR_LOG_INFO("Initializing flurr...");

  FLURR_LOG_INFO("flurr initialized.");
  initialized_ = true;
  return Status::kSuccess;
}

void FlurrCore::Shutdown() {
  FLURR_LOG_INFO("Shutting down flurr...");
  if (!initialized_) {
    FLURR_LOG_WARN("flurr not initialized!");
    return;
  }

  initialized_ = false;
  FLURR_LOG_INFO("flurr shutdown complete.");
}

Status FlurrCore::Update(float delta_time) {
  return Status::kSuccess;
}

FlurrCore& FlurrCore::Get() {
  static FlurrCore flurr_core;
  return flurr_core;
}

} // namespace flurr
