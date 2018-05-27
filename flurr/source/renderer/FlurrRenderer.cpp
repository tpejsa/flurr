#include "renderer/FlurrRenderer.h"
#include "FlurrCore.h"
#include "FlurrConfigFile.h"
#include "FlurrLog.h"

namespace flurr {

FlurrRenderer::FlurrRenderer()
  : initialized_(false) {
}

FlurrRenderer::~FlurrRenderer() {
  if (initialized_)
    Shutdown();
}

Status FlurrRenderer::Init(const std::string& config_path) {
  FLURR_LOG_INFO("Initializing flurr renderer...");

  if (!FlurrCore::Get().IsInitialized()) {
    FLURR_LOG_ERROR("Failed to initialize flurr renderer; flurr core not initialized!");
    return Status::kInitFailed;
  }

  FlurrCore::Get().SetRenderer(this);

  Status result = OnInit();
  if (Status::kSuccess != result) {
    FLURR_LOG_ERROR("Failed to initialize flurr renderer!");
    return result;
  }

  FLURR_LOG_INFO("flurr renderer initialized.");
  initialized_ = true;
  return result;
}

void FlurrRenderer::Shutdown() {
  FLURR_LOG_INFO("Shutting down flurr...");
  if (!initialized_) {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return;
  }

  OnShutdown();
  initialized_ = false;
  FLURR_LOG_INFO("flurr shutdown complete.");
}

Status FlurrRenderer::Update(float delta_time) {
  if (!initialized_) {
    FLURR_LOG_WARN("flurr renderer not initialized!");
    return Status::kNotInitialized;
  }

  return OnUpdate(delta_time);
}

} // namespace flurr
