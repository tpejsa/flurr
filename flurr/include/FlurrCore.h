#pragma once

#include "FlurrDefines.h"
#include "renderer/FlurrRenderer.h"

#include <string>

namespace flurr {

class FLURR_DLL_EXPORT FlurrCore {

private:

  FlurrCore();
  FlurrCore(const FlurrCore&) = delete;
  FlurrCore(FlurrCore&&) = delete;
  FlurrCore& operator=(const FlurrCore&) = delete;
  FlurrCore& operator=(FlurrCore&&) = delete;
  ~FlurrCore();

public:

  Status Init(const std::string& config_path = "flurr.cfg");
  void Shutdown();
  Status Update(float delta_time);
  inline bool IsInitialized() const { return initialized_; }
  inline FlurrRenderer* GetRenderer() const { return renderer_; }
  void SetRenderer(FlurrRenderer* renderer); // called by FlurrRenderer::Init

  static FlurrCore& Get();

private:

  bool initialized_;
  FlurrRenderer* renderer_;
};

} // namespace flurr
