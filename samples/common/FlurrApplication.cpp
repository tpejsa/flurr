#include "FlurrApplication.h"

#include <chrono>
#include <iostream>

namespace flurr {

FlurrApplication::FlurrApplication(int window_width, int window_height, const std::string& window_title)
  : window_width_(window_width > 0 ? window_width : 1024),
  window_height_(window_height > 0 ? window_height : 768),
  fullscreen_(window_width <= 0 || window_height <= 0),
  window_title_(window_title),
  window_(nullptr),
  shutdown_(false) {
}

int FlurrApplication::WindowWidth() const {
  return window_width_;
}

int FlurrApplication::WindowHeight() const {
  return window_height_;
}

bool FlurrApplication::IsFullScreen() const {
  return fullscreen_;
}

const std::string& FlurrApplication::WindowTitle() const {
  return window_title_;
}

GLFWwindow* FlurrApplication::GetWindow() const {
  return window_;
}

int FlurrApplication::Run() {
  FLURR_LOG_INFO("Initializing application...");

  // Initialize GLFW
  FLURR_LOG_INFO("Initializing GLFW...");
  if (!glfwInit()) {
    FLURR_LOG_ERROR("Failed to initialize GLFW!");
    return -1;
  }

  // Create application window
  FLURR_LOG_INFO("Creating application window (width: %d, height: %d, fullscreen: %d)...",
    WindowWidth(), WindowHeight(), IsFullScreen());
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window_ = glfwCreateWindow(
    WindowWidth(), WindowHeight(), window_title_.c_str(),
    IsFullScreen() ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  if (!window_) {
    FLURR_LOG_ERROR("Failed to create application window!");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window_);

  // Associate pointer to this FlurrApplication with the window
  // (needed in GLFW callbacks, which cannot be member functions)
  glfwSetWindowUserPointer(window_, this);

  // Initialize flurr
  FLURR_LOG_INFO("Initializing flurr...");
  auto& flurr_core = FlurrCore::Get();
  Status flurr_status = flurr_core.Init();
  if (flurr_status != Status::kSuccess) {
    FLURR_LOG_ERROR("flurr initialization failed (error: %u)!", FromEnum(flurr_status));
    glfwTerminate();
    return -1;
  }

  // Initialize flurr renderer
  FLURR_LOG_INFO("Initializing flurr renderer...");
  auto& flurr_renderer = FlurrOGLRenderer::Get();
  flurr_status = flurr_renderer.Init();
  if (flurr_status != Status::kSuccess) {
    FLURR_LOG_ERROR("flurr renderer initialization failed (error: %u)!", FromEnum(flurr_status));
    flurr_core.Shutdown();
    glfwTerminate();
    return -1;
  }

  // Register GLFW callbacks
  glfwSetFramebufferSizeCallback(window_, glfwFramebufferSizeCallback);
  glfwSetKeyCallback(window_, glfwKeyCallback);

  // Application-specific initialization
  FLURR_LOG_INFO("Application-specific initialization...");
  if (!OnInit()) {
    FLURR_LOG_ERROR("Application initialization failed!");
    flurr_core.Shutdown();
    glfwTerminate();
    return -1;
  }

  // Start timing
  auto time_start = std::chrono::high_resolution_clock::now();

  // Update loop
  bool shutdown = false;
  int result = 0;
  FLURR_LOG_INFO("Starting update loop...");
  while (!shutdown_) {
    // Compute delta time
    auto time_current = std::chrono::high_resolution_clock::now();
    float delta_time = std::chrono::duration_cast<std::chrono::seconds>(time_current - time_start).count();
    time_start = time_current;

    // Application-specific update
    if (!OnUpdate(delta_time))
      Quit();

    // Update flurr
    if (flurr_core.Update(delta_time) != Status::kSuccess) {
      Quit();
      result = -1;
      continue;
    }

    // Swap front and back buffers
    glfwSwapBuffers(window_);

    // Process events
    glfwPollEvents();

    // Has user closed window?
    if (glfwWindowShouldClose(window_) != 0)
      Quit();
  }

  // Application-specific cleanup
  FLURR_LOG_INFO("Application-specific cleanup...");
  OnQuit();

  // Shut down flurr
  FLURR_LOG_INFO("Shutting down flurr...");
  flurr_core.Shutdown();

  // Shut down GLFW
  FLURR_LOG_INFO("Shutting down GLFW...");
  glfwTerminate();

  return 0;
}

void FlurrApplication::Quit() {
  FLURR_LOG_INFO("Quitting application...");
  shutdown_ = true;
}

void FlurrApplication::UpdateCamera() {
  // TODO: update camera position and orientation based on user input
}

void FlurrApplication::OnKeyDown(int key) {
  switch (key) {
    case GLFW_KEY_ESCAPE: {
      Quit();
      break;
    }
    default: {
      break;
    }
  }
}

void FlurrApplication::OnKeyUp(int key) {
}

void FlurrApplication::glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  // Update renderer viewport size
  auto* renderer = FlurrCore::Get().GetRenderer();
  renderer->SetViewport(0, 0, width, height);

  // Update application window size
  app->window_width_ = width;
  app->window_height_ = height;
  app->OnWindowResize();
}

void FlurrApplication::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  auto* app = static_cast<FlurrApplication*>(glfwGetWindowUserPointer(window));
  FLURR_ASSERT(app, "GLFW window user pointer must be set to owning FlurrApplication!");

  if (!mods) {
    if (GLFW_PRESS == action) {
      app->OnKeyDown(key);
    }
    else if (GLFW_RELEASE == action) {
      app->OnKeyUp(key);
    }
  }
}

} // namespace flurr
