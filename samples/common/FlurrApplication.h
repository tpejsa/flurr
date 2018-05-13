#pragma once
#include <GLFW/glfw3.h>

#include <string>

namespace flurr {

class FlurrApplication {

public:

  FlurrApplication(int window_width = -1, int window_height = -1, const std::string& window_title = "FlurrApplication");
  FlurrApplication(const FlurrApplication&) = delete;
  FlurrApplication(FlurrApplication&&) = delete;
  FlurrApplication& operator=(const FlurrApplication&) = delete;
  FlurrApplication& operator=(FlurrApplication&&) = delete;
  virtual ~FlurrApplication() = default;

  inline int WindowWidth() const;
  inline int WindowHeight() const;
  inline bool IsFullScreen() const;
  inline const std::string& WindowTitle() const;
  inline GLFWwindow* GetWindow() const;
  int Run();
  void Quit();

private:

  virtual bool OnInit() = 0;
  virtual bool OnUpdate(float delta_time) = 0;
  virtual void OnQuit() = 0;

  // Application window
  int window_width_;
  int window_height_;
  bool fullscreen_;
  std::string window_title_;
  GLFWwindow* window_;

  // Application state
  bool shutdown_;
};

} // namespace flurr
