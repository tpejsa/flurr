#pragma once
#include <flurr.h>
#include <flogger.h>
#include <GLFW/glfw3.h>

#include <string>

namespace flurr
{

class FlurrApplication
{

public:

  FlurrApplication(int a_windowWidth = -1, int a_windowHeight = -1, const std::string& a_windowTitle = "FlurrApplication");
  FlurrApplication(const FlurrApplication&) = delete;
  FlurrApplication(FlurrApplication&&) = delete;
  FlurrApplication& operator=(const FlurrApplication&) = delete;
  FlurrApplication& operator=(FlurrApplication&&) = delete;
  virtual ~FlurrApplication() = default;

  int getWindowWidth() const;
  int getWindowHeight() const;
  bool isFullScreen() const;
  const std::string& getWindowTitle() const;
  GLFWwindow* getWindow() const;
  int run();
  void quit();

  ResourceManager* getResourceManager() const { return FlurrCore::Get().getResourceManager(); }
  SceneManager* getSceneManager() const { return FlurrCore::Get().getSceneManager(); }
  Node* getCameraNode() const { return getSceneManager()->getNode(m_camNodeHandle); }
  CameraComponent* getCamera() const { return static_cast<CameraComponent*>(getSceneManager()->getComponent(m_camHandle)); }
  void updateCameraTransform();

private:

  enum class CameraControlMode
  {
    kFixed = 0, // camera doesn't move, though you can still zoom using mouse scrollwheel
    kRotate, // camera rotates around the center
    kTranslate, // camera and center translate together parallel to the view plane
    kZoom // camera zooms (translates toward or away from the center)
  };

  bool initGlfw();
  bool createWindow();
  bool registerGlfwCallbacks();
  bool initFlurr();
  bool initCamera();
  bool initApp();
  void updateLoop();
  void shutdownApp();
  void shutdownFlurr();
  void shutdownGlfw();

  // Application-specific callbacks:
  virtual bool onInit() = 0;
  virtual bool onUpdate(float a_deltaTime) = 0;
  virtual void onDraw() {}
  virtual void onQuit() = 0;
  virtual void onWindowResize() {}
  virtual void onKeyDown(int a_key);
  virtual void onKeyUp(int a_key);
  virtual void onMouse(double a_x, double a_y);
  virtual void onMouseButtonDown(int a_button);
  virtual void onMouseButtonUp(int a_button);

  // Camera control functions:
  CameraControlMode getCameraControlMode() const { return m_camControlMode; }
  void setCameraControlMode(CameraControlMode a_camControlMode) { m_camControlMode = a_camControlMode; }
  const glm::vec3& getCameraCenter() const { return m_camCenter; }
  void setCameraCenter(const glm::vec3& a_camCenter) { m_camCenter = a_camCenter; }
  void startMouseMovement();
  void updateMouseMovement(double a_x, double a_y, double& a_deltaX, double& a_deltaY);
  void stopMouseMovement();

  // GLFW callbacks:
  static void glfwFramebufferSizeCallback(GLFWwindow* a_window, int a_width, int a_height);
  static void glfwKeyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);
  static void glfwMouseCallback(GLFWwindow* a_window, double a_x, double a_y);
  static void glfwMouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods);
  static void glfwScrollCallback(GLFWwindow* a_window, double a_x, double a_y);

  // Camera control constants  
  static constexpr float kMinCameraRadius = 0.05f;
  static constexpr float kCameraRotateSpeed = 0.01f;
  static constexpr float kCameraTranslateSpeed = 0.01f;
  static constexpr float kCameraZoomSpeed = 0.01f;
  static constexpr float kCameraZoomSpeedScroll = 1.0f;

  // Application window
  int m_windowWidth;
  int m_windowHeight;
  bool m_fullscreen;
  std::string m_windowTitle;
  GLFWwindow* m_window;

  // Application state
  bool m_shutdown;

  // Camera control
  FlurrHandle m_camNodeHandle;
  FlurrHandle m_camHandle;
  CameraControlMode m_camControlMode;
  glm::vec3 m_camCenter;
  float m_camRadius;
  float m_camElevationAngle;
  float m_camAzimuthAngle;

  // Mouse movement (for camera control)
  bool m_mouseMoving;
  double m_lastCursorX;
  double m_lastCursorY;
};

} // namespace flurr
