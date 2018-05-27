#pragma once

#include "FlurrDefines.h"

#include <string>

namespace flurr {

class FLURR_DLL_EXPORT FlurrRenderer {

protected:

  FlurrRenderer();
  FlurrRenderer(const FlurrRenderer&) = delete;
  FlurrRenderer(FlurrRenderer&&) = delete;
  FlurrRenderer& operator=(const FlurrRenderer&) = delete;
  FlurrRenderer& operator=(FlurrRenderer&&) = delete;
  virtual ~FlurrRenderer();

public:

  Status Init(const std::string& config_path = "flurr_renderer.cfg");
  void Shutdown();
  Status Update(float delta_time);
  inline bool IsInitialized() const { return initialized_; }
  virtual void SetViewport(int x, int y, uint32_t width, uint32_t height) = 0;

private:

  virtual Status OnInit() = 0;
  virtual void OnShutdown() = 0;
  virtual Status OnUpdate(float delta_time) = 0;

  bool initialized_;
};

} // namespace flurr
