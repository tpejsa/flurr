#include "flurr/FlurrCore.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/ConfigFile.h"

namespace flurr
{

FlurrCore::FlurrCore()
  : m_initialized(false),
  m_sceneManager(std::make_unique<SceneManager>()),
  m_resourceManager(std::make_unique<ResourceManager>()),
  m_renderer(std::make_unique<Renderer>())
{
}

FlurrCore::~FlurrCore()
{
  if (isInitialized())
    shutdown();
}

Status FlurrCore::init(const std::string& a_configPath)
{
  FLURR_LOG_INFO("Initializing flurr...");
  if (isInitialized())
  {
    FLURR_LOG_WARN("flurr already initialized!");
    return Status::kSuccess;
  }

  // TODO: load engine config

  // Initialize resource manager
  m_resourceManager->addResourceDirectory("./"); // TODO: condition this on a config setting
  if (!m_resourceManager->run())
  {
    FLURR_LOG_ERROR("Failed to start up ResourceManager thread!");
    return Status::kFailed;
  }

  // Initialize scene manager
  Status result = m_sceneManager->init();
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to initialize SceneManager!");
    return result;
  }

  // Initialize renderer
  result = m_renderer->init();
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to initialize Renderer!");
    return result;
  }

  FLURR_LOG_INFO("flurr initialized.");
  m_initialized = true;
  return Status::kSuccess;
}

void FlurrCore::shutdown()
{
  FLURR_LOG_INFO("Shutting down flurr...");
  if (!isInitialized())
  {
    FLURR_LOG_WARN("flurr not initialized!");
    return;
  }

  m_renderer->shutdown();
  m_sceneManager->shutdown();
  m_resourceManager->stop();
  m_resourceManager->removeAllResourceDirectories();

  m_initialized = false;
  FLURR_LOG_INFO("flurr shutdown complete.");
}

Status FlurrCore::update(float a_deltaTime)
{
  if (!isInitialized())
  {
    FLURR_LOG_ERROR("flurr not initialized!");
    return Status::kNotInitialized;
  }

  // Update resource listeners
  m_resourceManager->updateListeners();

  // Update scene
  Status result = m_sceneManager->update(a_deltaTime);
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to update flurr SceneManager!");
    return result;
  }

  // Update renderer
  if (m_renderer)
  {
    if (!m_renderer->isInitialized())
    {
      FLURR_LOG_ERROR("Failed to update flurr; renderer not initialized!");
      return Status::kNotInitialized;
    }

    result = m_renderer->update(a_deltaTime);
    if (Status::kSuccess != result)
    {
      FLURR_LOG_ERROR("Failed to update flurr renderer!");
      return result;
    }
  }

  // Draw the scene
  result = m_sceneManager->draw();
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to draw the scene!");
    return result;
  }

  return Status::kSuccess;
}

FlurrCore& FlurrCore::Get()
{
  static FlurrCore flurrCore;
  return flurrCore;
}

} // namespace flurr
