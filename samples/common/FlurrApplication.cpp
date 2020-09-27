#include "FlurrApplication.h"

#include <algorithm>
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

int FlurrApplication::getWindowWidth() const
{
  return m_windowWidth;
}

int FlurrApplication::getWindowHeight() const
{
  return m_windowHeight;
}

bool FlurrApplication::isFullScreen() const
{
  return m_fullscreen;
}

const std::string& FlurrApplication::getWindowTitle() const
{
  return m_windowTitle;
}

GLFWwindow* FlurrApplication::getWindow() const
{
  return m_window;
}

int FlurrApplication::run()
{
  FLURR_LOG_INFO("Initializing application...");

  if (!initGlfw())
    return -1;

  if (!createWindow())
    return -1;

  if (!registerGlfwCallbacks())
    return -1;

  if (!initFlurr())
    return -1;

  if (!initCamera())
    return -1;

  if (!initApp())
    return -1;

  // Enter main loop
  updateLoop();

  shutdownApp();
  shutdownFlurr();
  shutdownGlfw();

  return 0;
}

void FlurrApplication::quit()
{
  FLURR_LOG_INFO("Quitting application...");
  m_shutdown = true;
}

bool FlurrApplication::initGlfw()
{
  // Initialize GLFW
  FLURR_LOG_INFO("Initializing GLFW...");
  if (!glfwInit())
  {
    FLURR_LOG_ERROR("Failed to initialize GLFW!");
    return -1;
  }
}

bool FlurrApplication::createWindow()
{
  // Create application window
  FLURR_LOG_INFO("Creating application window (width: %d, height: %d, fullscreen: %d)...",
    getWindowWidth(), getWindowHeight(), isFullScreen());
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  m_window = glfwCreateWindow(
    getWindowWidth(), getWindowHeight(), m_windowTitle.c_str(),
    isFullScreen() ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  if (!m_window)
  {
    FLURR_LOG_ERROR("Failed to create application window!");
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(m_window);

  // Associate pointer to this FlurrApplication with the window
  // (needed in GLFW callbacks, which cannot be member functions)
  glfwSetWindowUserPointer(m_window, this);
}

bool FlurrApplication::registerGlfwCallbacks()
{
  // Register GLFW callbacks
  glfwSetFramebufferSizeCallback(m_window, glfwFramebufferSizeCallback);
  glfwSetKeyCallback(m_window, glfwKeyCallback);
  glfwSetCursorPosCallback(m_window, glfwMouseCallback);
  glfwSetMouseButtonCallback(m_window, glfwMouseButtonCallback);
  glfwSetScrollCallback(m_window, glfwScrollCallback);

  return true;
}

bool FlurrApplication::initFlurr()
{
  // Initialize flurr
  FLURR_LOG_INFO("Initializing flurr...");
  auto& flurrCore = FlurrCore::Get();
  Status flurrStatus = flurrCore.init();
  if (flurrStatus != Status::kSuccess)
  {
    FLURR_LOG_ERROR("flurr initialization failed (error: %u)!", FromEnum(flurrStatus));
    glfwTerminate();
    return false;
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
    return false;
  }
}

bool FlurrApplication::initCamera()
{
  auto* sceneManager = getSceneManager();
  if (Status::kSuccess != sceneManager->createNode(m_camNodeHandle, "Camera"))
  {
    FLURR_LOG_ERROR("Failed to create camera node!");
    return false;
  }

  CameraComponentInitArgs camera_init_args;
  camera_init_args.vpw = getWindowWidth();
  camera_init_args.vph = getWindowHeight();
  auto result = sceneManager->createComponent(m_camHandle, m_camNodeHandle, camera_init_args);
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create camera component!");
    return false;
  }

  // Set initial camera positioning
  auto* camNode = getCameraNode();
  camNode->setPosition({1.0f, 0.0f, 0.0f});
  camNode->lookAt(MathUtils::ZERO);

  // Initially the camera is fixed
  m_camControlMode = CameraControlMode::kFixed;
  m_camCenter = MathUtils::ZERO;

  return true;
}

bool FlurrApplication::initApp()
{
  // Application-specific initialization
  FLURR_LOG_INFO("Application-specific initialization...");
  if (!onInit())
  {
    FLURR_LOG_ERROR("Application initialization failed!");
    FlurrCore::Get().shutdown();
    glfwTerminate();
    return -1;
  }
}

void FlurrApplication::updateLoop()
{
  // Start timing
  auto timeStart = std::chrono::high_resolution_clock::now();

  // Update loop
  FLURR_LOG_INFO("Starting update loop...");
  while (!m_shutdown)
  {
    // Compute delta time
    auto timeCurrent = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(timeCurrent - timeStart).count();
    timeStart = timeCurrent;

    // Application-specific update
    if (!onUpdate(deltaTime))
      quit();

    // Update flurr
    if (FlurrCore::Get().update(deltaTime) != Status::kSuccess)
    {
      quit();
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
      quit();
  }
}

void FlurrApplication::shutdownApp()
{
  // Application-specific cleanup
  FLURR_LOG_INFO("Application-specific cleanup...");
  onQuit();
}

void FlurrApplication::shutdownFlurr()
{
  // Shut down flurr
  FLURR_LOG_INFO("Shutting down flurr...");
  FlurrCore::Get().shutdown();
}

void FlurrApplication::shutdownGlfw()
{
  // Shut down GLFW
  FLURR_LOG_INFO("Shutting down GLFW...");
  glfwTerminate();
}

void FlurrApplication::onKeyDown(int a_key)
{
}

void FlurrApplication::onKeyUp(int a_key)
{
}

void FlurrApplication::onMouse(double a_x, double a_y)
{
}

void FlurrApplication::onMouseButtonDown(int a_button)
{
}

void FlurrApplication::onMouseButtonUp(int a_button)
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
      switch (a_key) {
        case GLFW_KEY_ESCAPE:
        {
          app->quit();
          return;
        }
        default:
        {
          break;
        }
      }

      app->onKeyDown(a_key);
    }
    else if (GLFW_RELEASE == a_action)
    {
      app->onKeyUp(a_key);
    }
  }
}

void FlurrApplication::glfwMouseCallback(GLFWwindow* a_window, double a_x, double a_y)
{
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(a_window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  if (CameraControlMode::kRotate == app->getCameraControlMode())
  {
    // TODO: rotate camera node around the center, use kCameraRotateSpeed
  }
  else if (CameraControlMode::kTranslate == app->getCameraControlMode())
  {
    // TODO: translate camera node and center in view plane, kCameraTranslateSpeed
  }
}

void FlurrApplication::glfwMouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(a_window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  if (!a_mods)
  {
    if (GLFW_PRESS == a_action)
    {
      if (GLFW_MOUSE_BUTTON_MIDDLE == a_button)
      {
        if (a_mods & GLFW_MOD_ALT)
          app->setCameraControlMode(CameraControlMode::kTranslate);
        else
          app->setCameraControlMode(CameraControlMode::kRotate);

        return;
      }

      app->onMouseButtonDown(a_button);
    }
    else if (GLFW_RELEASE == a_action)
    {
      if (GLFW_MOUSE_BUTTON_MIDDLE == a_button)
      {
        app->setCameraControlMode(CameraControlMode::kFixed);
        return;
      }

      app->onMouseButtonUp(a_button);
    }
  }
}

void FlurrApplication::glfwScrollCallback(GLFWwindow* a_window, double a_x, double a_y)
{
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(a_window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

	if (CameraControlMode::kFixed == app->getCameraControlMode())
	{
		auto* camNode = app->getCameraNode();
    const float camCenterDistance = glm::distance(camNode->getWorldPosition(), app->getCameraCenter());
    const float camTranslateDelta = kCameraZoomSpeed*a_y;
    const float clampedCamTranslateDelta = camTranslateDelta > 0.0f ? std::min(camTranslateDelta, camCenterDistance - kCameraZoomEpsilon) : camTranslateDelta;

    camNode->translate(clampedCamTranslateDelta*MathUtils::FORWARD);
	}
}

} // namespace flurr
