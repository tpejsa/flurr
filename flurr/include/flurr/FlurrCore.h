#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/renderer/Renderer.h"
#include "flurr/resource/ResourceManager.h"
#include "flurr/scene/SceneManager.h"

#include <memory>
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
  ResourceManager* getResourceManager() const { return m_resourceManager.get(); }
  SceneManager* getSceneManager() const { return m_sceneManager.get(); }
  Renderer* getRenderer() const { return m_renderer.get(); }

  static FlurrCore& Get();

private:

  bool m_initialized;
  std::unique_ptr<ResourceManager> m_resourceManager;
  std::unique_ptr<SceneManager> m_sceneManager;
  std::unique_ptr<Renderer> m_renderer;
};

} // namespace flurr
