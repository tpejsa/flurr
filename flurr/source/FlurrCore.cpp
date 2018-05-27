#include "FlurrCore.h"
#include "FlurrConfigFile.h"
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

  if (renderer_) {
    if (renderer_->IsInitialized())
      renderer_->Shutdown();
    renderer_ = nullptr;
  }

  initialized_ = false;
  FLURR_LOG_INFO("flurr shutdown complete.");
}

Status FlurrCore::Update(float delta_time) {
  Status result = Status::kSuccess;
  if (renderer_) {
    if (!renderer_->IsInitialized()) {
      FLURR_LOG_ERROR("Failed to update flurr; renderer not initialized!");
      return Status::kNotInitialized;
    }

    result = renderer_->Update(delta_time);
  }

  return result;
}

void FlurrCore::SetRenderer(FlurrRenderer* renderer) {
  if (renderer)
    FLURR_LOG_INFO("flurr renderer set.");
  else
    FLURR_LOG_INFO("flurr renderer unset.");
  renderer_ = renderer;
}

FlurrCore& FlurrCore::Get() {
  static FlurrCore flurr_core;
  return flurr_core;
}

} // namespace flurr
