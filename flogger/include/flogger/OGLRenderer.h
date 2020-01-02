#pragma once

#include "flurr.h"

#include <GL/glew.h>
#include <string>

namespace flurr
{

class FLURR_DLL_EXPORT FlurrOGLRenderer : public FlurrRenderer
{

private:

  FlurrOGLRenderer();
  ~FlurrOGLRenderer();

public:

  void setViewport(int a_x, int a_y, uint32_t a_width, uint32_t a_height) override;

  static FlurrOGLRenderer& Get();

private:

  Status onInit() override;
  void onShutdown() override;
  Status onUpdate(float a_deltaTime) override;

  ShaderProgram* onCreateShaderProgram(FlurrHandle a_programHandle) override;
  Texture* onCreateTexture(FlurrHandle a_texHandle) override;
  VertexBuffer* onCreateVertexBuffer(FlurrHandle a_bufferHandle) override;
  VertexArray* onCreateVertexArray(FlurrHandle a_arrayHandle) override;
};

} // namespace flurr
