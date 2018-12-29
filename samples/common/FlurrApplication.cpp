#include "FlurrApplication.h"

#include <chrono>
#include <iostream>

namespace flurr
{

FlurrApplication::FlurrApplication(int a_windowWidth, int a_windowHeight, const std::string& a_windowTitle)
  : m_windowWidth(a_windowWidth > 0 ? a_windowWidth : 1024),
  m_windowHeight(a_windowHeight > 0 ? a_windowHeight : 768),
  m_fullscreen(a_windowWidth <= 0 || a_windowHeight <= 0),
  m_windowTitle(a_windowTitle),
  m_window(nullptr),
  m_shutdown(false)
{
}

int FlurrApplication::WindowWidth() const
{
  return m_windowWidth;
}

int FlurrApplication::WindowHeight() const
{
  return m_windowHeight;
}

bool FlurrApplication::IsFullScreen() const
{
  return m_fullscreen;
}

const std::string& FlurrApplication::WindowTitle() const
{
  return m_windowTitle;
}

GLFWwindow* FlurrApplication::GetWindow() const
{
  return m_window;
}

int FlurrApplication::Run()
{
  FLURR_LOG_INFO("Initializing application...");

  // Initialize GLFW
  FLURR_LOG_INFO("Initializing GLFW...");
  if (!glfwInit())
  {
    FLURR_LOG_ERROR("Failed to initialize GLFW!");
    return -1;
  }

  // Create application window
  FLURR_LOG_INFO("Creating application window (width: %d, height: %d, fullscreen: %d)...",
    WindowWidth(), WindowHeight(), IsFullScreen());
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  m_window = glfwCreateWindow(
    WindowWidth(), WindowHeight(), m_windowTitle.c_str(),
    IsFullScreen() ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  if (!m_window)
  {
    FLURR_LOG_ERROR("Failed to create application window!");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(m_window);

  // Associate pointer to this FlurrApplication with the window
  // (needed in GLFW callbacks, which cannot be member functions)
  glfwSetWindowUserPointer(m_window, this);

  // Initialize flurr
  FLURR_LOG_INFO("Initializing flurr...");
  auto& flurrCore = FlurrCore::Get();
  Status flurrStatus = flurrCore.init();
  if (flurrStatus != Status::kSuccess)
  {
    FLURR_LOG_ERROR("flurr initialization failed (error: %u)!", FromEnum(flurrStatus));
    glfwTerminate();
    return -1;
  }

  // Initialize flurr renderer
  FLURR_LOG_INFO("Initializing flurr renderer...");
  auto& flurrRenderer = FlurrOGLRenderer::Get();
  flurrStatus = flurrRenderer.init();
  if (flurrStatus != Status::kSuccess)
  {
    FLURR_LOG_ERROR("flurr renderer initialization failed (error: %u)!", FromEnum(flurrStatus));
    flurrCore.shutdown();
    glfwTerminate();
    return -1;
  }

  // Register GLFW callbacks
  glfwSetFramebufferSizeCallback(m_window, glfwFramebufferSizeCallback);
  glfwSetKeyCallback(m_window, glfwKeyCallback);

  // Application-specific initialization
  FLURR_LOG_INFO("Application-specific initialization...");
  if (!onInit())
  {
    FLURR_LOG_ERROR("Application initialization failed!");
    flurrCore.shutdown();
    glfwTerminate();
    return -1;
  }

  // Start timing
  auto timeStart = std::chrono::high_resolution_clock::now();

  // Update loop
  bool shutdown = false;
  int result = 0;
  FLURR_LOG_INFO("Starting update loop...");
  while (!m_shutdown)
  {
    // Compute delta time
    auto timeCurrent = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::seconds>(timeCurrent - timeStart).count();
    timeStart = timeCurrent;

    // Application-specific update
    if (!onUpdate(deltaTime))
      Quit();

    // Update flurr
    if (flurrCore.update(deltaTime) != Status::kSuccess)
    {
      Quit();
      result = -1;
      continue;
    }

    // Application-specific drawing
    onDraw();

    // Swap front and back buffers
    glfwSwapBuffers(m_window);

    // Process events
    glfwPollEvents();

    // Has user closed window?
    if (glfwWindowShouldClose(m_window) != 0)
      Quit();
  }

  // Application-specific cleanup
  FLURR_LOG_INFO("Application-specific cleanup...");
  onQuit();

  // Shut down flurr
  FLURR_LOG_INFO("Shutting down flurr...");
  flurrCore.shutdown();

  // Shut down GLFW
  FLURR_LOG_INFO("Shutting down GLFW...");
  glfwTerminate();

  return 0;
}

void FlurrApplication::Quit()
{
  FLURR_LOG_INFO("Quitting application...");
  m_shutdown = true;
}

void FlurrApplication::UpdateCamera()
{
  // TODO: update camera position and orientation based on user input
}

void FlurrApplication::onKeyDown(int a_key)
{
  switch (a_key)
  {
    case GLFW_KEY_ESCAPE:
    {
      Quit();
      break;
    }
    default:
    {
      break;
    }
  }
}

void FlurrApplication::onKeyUp(int a_key)
{
}

void FlurrApplication::glfwFramebufferSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(a_window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  // Update renderer viewport size
  auto* renderer = FlurrCore::Get().getRenderer();
  renderer->setViewport(0, 0, a_width, a_height);

  // Update application window size
  app->m_windowWidth = a_width;
  app->m_windowHeight = a_height;
  app->onWindowResize();
}

void FlurrApplication::glfwKeyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(a_window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  if (!a_mods)
  {
    if (GLFW_PRESS == a_action)
    {
      app->onKeyDown(a_key);
    }
    else if (GLFW_RELEASE == a_action)
    {
      app->onKeyUp(a_key);
    }
  }
}

} // namespace flurr
