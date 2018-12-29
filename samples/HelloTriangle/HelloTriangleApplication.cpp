#include "HelloTriangleApplication.h"

//
#include <fstream>
#include <streambuf>
//

namespace flurr
{

HelloTriangleApplication::HelloTriangleApplication(int a_windowWidth, int a_windowHeight)
  : FlurrApplication(a_windowWidth, a_windowHeight, "HelloTriangle"),
  m_shaderProgramHandle(INVALID_OBJECT),
  m_vaHandle(INVALID_OBJECT)
{
}

bool HelloTriangleApplication::onInit()
{
  // Compile and link vertex color shaders
  auto* renderer = FlurrCore::Get().getRenderer();
  m_shaderProgramHandle = renderer->createShaderProgram();
  auto* shaderProgram = renderer->getShaderProgram(m_shaderProgramHandle);
  if (Status::kSuccess != shaderProgram->compileShader(ShaderType::kVertex, kVertexShaderPath))
  {
    FLURR_LOG_ERROR("Failed to compile vertex shader %s!", kVertexShaderPath);
    return false;
  }
  if (Status::kSuccess != shaderProgram->compileShader(ShaderType::kFragment, kFragmentShaderPath))
  {
    FLURR_LOG_ERROR("Failed to compile fragment shader %s!", kFragmentShaderPath);
    return false;
  }
  if (Status::kSuccess != shaderProgram->linkProgram())
  {
    FLURR_LOG_ERROR("Failed to link shader program!");
    return false;
  }

  // Create vertex buffers for a pair of triangles
  m_vb1Handle = renderer->createVertexBuffer();
  auto* vb1 = renderer->getVertexBuffer(m_vb1Handle);
  if (Status::kSuccess != vb1->createBuffer(VertexBufferType::kVertexAttribute,
    sizeof(kVertexData), const_cast<float*>(&kVertexData[0]), 3*sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for positions!");
    return false;
  }
  m_vb2Handle = renderer->createVertexBuffer();
  auto* vb2 = renderer->getVertexBuffer(m_vb2Handle);
  if (Status::kSuccess != vb2->createBuffer(VertexBufferType::kVertexAttribute,
    sizeof(kColorData), const_cast<float*>(&kColorData[0]), 3*sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for colors!");
    return false;
  }
  m_ibHandle = renderer->createVertexBuffer();
  auto* ib = renderer->getVertexBuffer(m_ibHandle);
  if (Status::kSuccess != ib->createIndexBuffer(
    sizeof(kIndexData), const_cast<uint32_t*>(&kIndexData[0])))
  {
    FLURR_LOG_ERROR("Failed to create index buffer!");
    return false;
  }

  // Create vertex array for our geometry
  m_vaHandle = renderer->createVertexArray();
  auto* va = renderer->getVertexArray(m_vaHandle);
  va->addAttributeBuffer(m_vb1Handle);
  va->addAttributeBuffer(m_vb2Handle);
  va->setIndexBuffer(m_ibHandle);
  if (Status::kSuccess != va->createArray())
  {
    FLURR_LOG_ERROR("Failed to create vertex array!");
    return false;
  }

  return true;
}

bool HelloTriangleApplication::onUpdate(float a_deltaTime)
{
  return true;
}

void HelloTriangleApplication::onDraw()
{
  auto* renderer = FlurrCore::Get().getRenderer();
  auto* shaderProgram = renderer->getShaderProgram(m_shaderProgramHandle);
  shaderProgram->useProgram();
  auto* va = renderer->getVertexArray(m_vaHandle);
  va->drawArray();
}

void HelloTriangleApplication::onQuit()
{
  auto* renderer = FlurrCore::Get().getRenderer();
  renderer->destroyVertexArray(m_vaHandle);
  renderer->destroyVertexBuffer(m_vb1Handle);
  renderer->destroyVertexBuffer(m_vb2Handle);
  renderer->destroyVertexBuffer(m_ibHandle);
  renderer->destroyShaderProgram(m_shaderProgramHandle);
}

} // namespace flurr
