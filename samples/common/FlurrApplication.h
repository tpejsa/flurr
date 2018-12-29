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

  int WindowWidth() const;
  int WindowHeight() const;
  bool IsFullScreen() const;
  const std::string& WindowTitle() const;
  GLFWwindow* GetWindow() const;
  int Run();
  void Quit();

private:

  void UpdateCamera();

  // Application-specific callbacks:
  virtual bool onInit() = 0;
  virtual bool onUpdate(float a_deltaTime) = 0;
  virtual void onDraw() {}
  virtual void onQuit() = 0;
  virtual void onWindowResize() {}
  virtual void onKeyDown(int a_key);
  virtual void onKeyUp(int a_key);

  // GLFW callbacks:
  static void glfwFramebufferSizeCallback(GLFWwindow* a_window, int a_width, int a_height);
  static void glfwKeyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

  // Application window
  int m_windowWidth;
  int m_windowHeight;
  bool m_fullscreen;
  std::string m_windowTitle;
  GLFWwindow* m_window;

  // Application state
  bool m_shutdown;
};

} // namespace flurr
