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

void FlurrApplication::updateCameraTransform()
{
  auto* camNode = getCameraNode();
  const auto camRot = glm::angleAxis(m_camElevationAngle, MathUtils::RIGHT) * glm::angleAxis(m_camAzimuthAngle, MathUtils::UP);
  const auto camPos = m_camCenter - m_camRadius * glm::rotate(camRot, MathUtils::FORWARD);
  camNode->setRotation(camRot);
  camNode->setPosition(camPos);
}

bool FlurrApplication::initGlfw()
{
  // Initialize GLFW
  FLURR_LOG_INFO("Initializing GLFW...");
  if (!glfwInit())
  {
    FLURR_LOG_ERROR("Failed to initialize GLFW!");
    return false;
  }

  return true;
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

  return true;
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

  return true;
}

bool FlurrApplication::initCamera()
{
  auto* sceneManager = getSceneManager();
  if (Status::kSuccess != sceneManager->createNode(m_camNodeHandle, "Camera"))
  {
    FLURR_LOG_ERROR("Failed to create camera node!");
    return false;
  }

  CameraComponentInitArgs cameraInitArgs;
  cameraInitArgs.vpw = getWindowWidth();
  cameraInitArgs.vph = getWindowHeight();
  auto result = sceneManager->createComponent(m_camHandle, m_camNodeHandle, cameraInitArgs);
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create camera component!");
    return false;
  }
  
  // Initially the camera is fixed
  m_camControlMode = CameraControlMode::kFixed;
  m_camCenter = MathUtils::ZERO;
  m_camRadius = 3.0f;
  m_camElevationAngle = 0.0f;
  m_camAzimuthAngle = 0.0f;
  updateCameraTransform();

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
    return false;
  }

  return true;
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

void FlurrApplication::startMouseMovement()
{
  m_mouseMoving = true;
  glfwGetCursorPos(getWindow(), &m_lastCursorX, &m_lastCursorY);
}

void FlurrApplication::updateMouseMovement(double a_x, double a_y, double& a_deltaX, double& a_deltaY)
{
  if (!m_mouseMoving)
    return;

  a_deltaX = a_x - m_lastCursorX;
  a_deltaY = a_y - m_lastCursorY;
  m_lastCursorX = a_x;
  m_lastCursorY = a_y;
}

void FlurrApplication::stopMouseMovement()
{
  m_mouseMoving = false;
}

void FlurrApplication::glfwFramebufferSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(a_window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  // Update viewport
  auto* sceneManager = FlurrCore::Get().getSceneManager();
  auto* activeCamera = sceneManager->getActiveCamera();
  activeCamera->setViewport(0, 0, a_width, a_height);
  activeCamera->applyRendererViewport();

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
    
  if (auto camControlMode = app->getCameraControlMode(); camControlMode != CameraControlMode::kFixed)
  {
    // Calculate mouse movement delta
    double deltaX = 0.0;
    double deltaY = 0.0;
    app->updateMouseMovement(a_x, a_y, deltaX, deltaY);

    if (CameraControlMode::kRotate == camControlMode)
    {
      // Update camera elevation and azimuth angles
      const float azimuthAngleDelta = kCameraRotateSpeed * (float)deltaX;
      app->m_camAzimuthAngle = MathUtils::MapAnglePi(app->m_camAzimuthAngle + azimuthAngleDelta);
      const float elevationAngleDelta = kCameraRotateSpeed * (float)deltaY;
      app->m_camElevationAngle = std::clamp(
        MathUtils::MapAnglePi(app->m_camElevationAngle + elevationAngleDelta),
        -glm::half_pi<float>(),
        glm::half_pi<float>()
      );
    }
    else if (CameraControlMode::kTranslate == camControlMode)
    {
      // Translate the camera center along the view plane
      const auto centerDelta = kCameraTranslateSpeed * glm::vec3((float)-deltaX, (float)deltaY, 0.0f);
      auto* camNode = app->getCameraNode();
      app->m_camCenter += glm::rotate(camNode->getRotation(), centerDelta);
    }
    else if (CameraControlMode::kZoom == camControlMode)
    {
      // Update camera radius (distance from center)
      const float camZoomDelta = -kCameraZoomSpeed * (float)deltaY;
      app->m_camRadius = std::max(app->m_camRadius + camZoomDelta, app->kMinCameraRadius);
    }

    app->updateCameraTransform();
  }
}

void FlurrApplication::glfwMouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(a_window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  if (auto camControlMode = app->getCameraControlMode(); CameraControlMode::kFixed == camControlMode)
  {
    if (GLFW_PRESS == a_action && (a_mods & GLFW_MOD_ALT))
    {
      switch (a_button) {
      case GLFW_MOUSE_BUTTON_MIDDLE:
        app->setCameraControlMode(CameraControlMode::kTranslate);
        app->startMouseMovement();
        return;
      case GLFW_MOUSE_BUTTON_LEFT:
        app->setCameraControlMode(CameraControlMode::kRotate);
        app->startMouseMovement();
        return;
      case GLFW_MOUSE_BUTTON_RIGHT:
        app->setCameraControlMode(CameraControlMode::kZoom);
        app->startMouseMovement();
        return;
      default:
        break;
      }
    }
  }
  else if (GLFW_RELEASE == a_action &&
           (CameraControlMode::kTranslate == camControlMode && GLFW_MOUSE_BUTTON_MIDDLE == a_button ||
           CameraControlMode::kRotate == camControlMode && GLFW_MOUSE_BUTTON_LEFT == a_button ||
           CameraControlMode::kZoom == camControlMode && GLFW_MOUSE_BUTTON_RIGHT == a_button))
  {
    app->setCameraControlMode(CameraControlMode::kFixed);
    app->stopMouseMovement();
    return;
  }

  if (GLFW_PRESS == a_action)
  {
    app->onMouseButtonDown(a_button);
  }
  else if (GLFW_RELEASE == a_action)
  {
    app->onMouseButtonUp(a_button);
  }
}

void FlurrApplication::glfwScrollCallback(GLFWwindow* a_window, double a_x, double a_y)
{
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(a_window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  if (CameraControlMode::kFixed == app->getCameraControlMode())
  {
    const float camZoomDelta = -kCameraZoomSpeedScroll * (float)a_y;
    app->m_camRadius = std::max(app->m_camRadius + camZoomDelta, app->kMinCameraRadius);
    app->updateCameraTransform();
  }
}

} // namespace flurr
