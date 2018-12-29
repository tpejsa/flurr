#include "flurr/FlurrCore.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/ConfigFile.h"

namespace flurr
{

FlurrCore::FlurrCore()
  : m_initialized(false)
{
}

FlurrCore::~FlurrCore()
{
  if (m_initialized)
    shutdown();
}

Status FlurrCore::init(const std::string& a_configPath)
{
  FLURR_LOG_INFO("Initializing flurr...");

  FLURR_LOG_INFO("flurr initialized.");
  m_initialized = true;
  return Status::kSuccess;
}

void FlurrCore::shutdown()
{
  FLURR_LOG_INFO("Shutting down flurr...");
  if (!m_initialized)
  {
    FLURR_LOG_WARN("flurr not initialized!");
    return;
  }

  if (m_renderer)
  {
    if (m_renderer->isInitialized())
      m_renderer->shutdown();
    m_renderer = nullptr;
  }

  m_initialized = false;
  FLURR_LOG_INFO("flurr shutdown complete.");
}

Status FlurrCore::update(float a_deltaTime)
{
  Status result = Status::kSuccess;
  if (m_renderer)
  {
    if (!m_renderer->isInitialized())
    {
      FLURR_LOG_ERROR("Failed to update flurr; renderer not initialized!");
      return Status::kNotInitialized;
    }

    result = m_renderer->update(a_deltaTime);
  }

  return result;
}

void FlurrCore::setRenderer(FlurrRenderer* a_renderer)
{
  if (a_renderer)
    FLURR_LOG_INFO("flurr renderer set.");
  else
    FLURR_LOG_INFO("flurr renderer unset.");
  m_renderer = a_renderer;
}

FlurrCore& FlurrCore::Get()
{
  static FlurrCore flurrCore;
  return flurrCore;
}

} // namespace flurr
