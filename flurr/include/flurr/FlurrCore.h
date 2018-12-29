#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/renderer/Renderer.h"

#include <string>

namespace flurr
{

class FLURR_DLL_EXPORT FlurrCore
{

private:

  FlurrCore();
  FlurrCore(const FlurrCore&) = delete;
  FlurrCore(FlurrCore&&) = delete;
  FlurrCore& operator=(const FlurrCore&) = delete;
  FlurrCore& operator=(FlurrCore&&) = delete;
  ~FlurrCore();

public:

  Status init(const std::string& a_configPath = "flurr.cfg");
  void shutdown();
  Status update(float a_deltaTime);
  bool isInitialized() const { return m_initialized; }
  FlurrRenderer* getRenderer() const { return m_renderer; }
  void setRenderer(FlurrRenderer* a_renderer); // called by FlurrRenderer::init

  static FlurrCore& Get();

private:

  bool m_initialized;
  FlurrRenderer* m_renderer;
};

} // namespace flurr
