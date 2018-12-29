#include "HelloTriangleApplication.h"

#include <cmath>
#include <glm/gtc/constants.hpp>

namespace flurr
{

HelloTriangleApplication::HelloTriangleApplication(int a_windowWidth, int a_windowHeight)
  : FlurrApplication(a_windowWidth, a_windowHeight, "HelloTriangle"),
  m_sp1Handle(INVALID_OBJECT),
  m_vb1PosHandle(INVALID_OBJECT),
  m_vb1ColorHandle(INVALID_OBJECT),
  m_ib1Handle(INVALID_OBJECT),
  m_va1Handle(INVALID_OBJECT),
  m_sp2Handle(INVALID_OBJECT),
  m_vb2PosHandle(INVALID_OBJECT),
  m_ib2Handle(INVALID_OBJECT),
  m_va2Handle(INVALID_OBJECT),
  m_albedoTime(0.0f)
{
}

bool HelloTriangleApplication::onInit()
{
  // Compile and link vertex color shaders
  auto* renderer = FlurrCore::Get().getRenderer();
  m_sp1Handle = renderer->createShaderProgram();
  auto* sp1 = renderer->getShaderProgram(m_sp1Handle);
  if (Status::kSuccess != sp1->compileShader(ShaderType::kVertex, kVS1Path))
  {
    FLURR_LOG_ERROR("Failed to compile vertex shader %s 1!", kVS1Path);
    return false;
  }
  if (Status::kSuccess != sp1->compileShader(ShaderType::kFragment, kFS1Path))
  {
    FLURR_LOG_ERROR("Failed to compile fragment shader %s 1!", kFS1Path);
    return false;
  }
  if (Status::kSuccess != sp1->linkProgram())
  {
    FLURR_LOG_ERROR("Failed to link shader program 1!");
    return false;
  }

  // Create vertex buffers for geometry 1
  m_vb1PosHandle = renderer->createVertexBuffer();
  auto* vb1Pos = renderer->getVertexBuffer(m_vb1PosHandle);
  if (Status::kSuccess != vb1Pos->createBuffer(VertexBufferType::kVertexAttribute,
    sizeof(kPositionData1), const_cast<float*>(&kPositionData1[0]), 3*sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for positions 1!");
    return false;
  }
  m_vb1ColorHandle = renderer->createVertexBuffer();
  auto* vb1Color = renderer->getVertexBuffer(m_vb1ColorHandle);
  if (Status::kSuccess != vb1Color->createBuffer(VertexBufferType::kVertexAttribute,
    sizeof(kColorData1), const_cast<float*>(&kColorData1[0]), 3*sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for colors 1!");
    return false;
  }
  m_ib1Handle = renderer->createVertexBuffer();
  auto* ib1 = renderer->getVertexBuffer(m_ib1Handle);
  if (Status::kSuccess != ib1->createIndexBuffer(
    sizeof(kIndexData1), const_cast<uint32_t*>(&kIndexData1[0])))
  {
    FLURR_LOG_ERROR("Failed to create index buffer 1!");
    return false;
  }

  // Create vertex array for geometry 1
  m_va1Handle = renderer->createVertexArray();
  auto* va1 = renderer->getVertexArray(m_va1Handle);
  va1->addAttributeBuffer(m_vb1PosHandle);
  va1->addAttributeBuffer(m_vb1ColorHandle);
  va1->setIndexBuffer(m_ib1Handle);
  if (Status::kSuccess != va1->createArray())
  {
    FLURR_LOG_ERROR("Failed to create vertex array 1!");
    return false;
  }

  // Compile and link Phong shaders
  m_sp2Handle = renderer->createShaderProgram();
  auto* sp2 = renderer->getShaderProgram(m_sp2Handle);
  if (Status::kSuccess != sp2->compileShader(ShaderType::kVertex, kVS2Path))
  {
    FLURR_LOG_ERROR("Failed to compile vertex shader %s 2!", kVS2Path);
    return false;
  }
  if (Status::kSuccess != sp2->compileShader(ShaderType::kFragment, kFS2Path))
  {
    FLURR_LOG_ERROR("Failed to compile fragment shader %s 2!", kFS2Path);
    return false;
  }
  if (Status::kSuccess != sp2->linkProgram())
  {
    FLURR_LOG_ERROR("Failed to link shader program 1!");
    return false;
  }

  // Create vertex buffers for geometry 2
  m_vb2PosHandle = renderer->createVertexBuffer();
  auto* vb2 = renderer->getVertexBuffer(m_vb2PosHandle);
  if (Status::kSuccess != vb2->createBuffer(VertexBufferType::kVertexAttribute,
    sizeof(kPositionData2), const_cast<float*>(&kPositionData2[0]), 3 * sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for positions 2!");
    return false;
  }
  m_ib2Handle = renderer->createVertexBuffer();
  auto* ib2 = renderer->getVertexBuffer(m_ib2Handle);
  if (Status::kSuccess != ib2->createIndexBuffer(
    sizeof(kIndexData2), const_cast<uint32_t*>(&kIndexData2[0])))
  {
    FLURR_LOG_ERROR("Failed to create index buffer 2!");
    return false;
  }

  // Create vertex array for geometry 2
  m_va2Handle = renderer->createVertexArray();
  auto* va2 = renderer->getVertexArray(m_va2Handle);
  va2->addAttributeBuffer(m_vb2PosHandle);
  va2->setIndexBuffer(m_ib2Handle);
  if (Status::kSuccess != va2->createArray())
  {
    FLURR_LOG_ERROR("Failed to create vertex array 2!");
    return false;
  }

  return true;
}

bool HelloTriangleApplication::onUpdate(float a_deltaTime)
{
  m_albedoTime += a_deltaTime;
  return true;
}

void HelloTriangleApplication::onDraw()
{
  // Draw geometry 1
  auto* renderer = FlurrCore::Get().getRenderer();
  auto* sp1 = renderer->getShaderProgram(m_sp1Handle);
  sp1->useProgram();
  auto* va1 = renderer->getVertexArray(m_va1Handle);
  va1->drawArray();

  // Draw geometry 2
  auto* sp2 = renderer->getShaderProgram(m_sp2Handle);
  sp2->useProgram();
  const glm::vec3 albedo(
    sin(m_albedoTime*glm::pi<float>()/2.0f)/2.0f + 0.5f,
    sin((m_albedoTime/2.0f + 0.25f)*glm::pi<float>())/2.0f + 0.5f,
    sin((m_albedoTime/2.0f + 0.5f)*glm::pi<float>())/2.0f + 0.5f);
  if (!sp2->setVec3Value("albedo", albedo))
    FLURR_LOG_ERROR("Failed to resolve uniform albedo!");
  auto* va2 = renderer->getVertexArray(m_va2Handle);
  va2->drawArray();
}

void HelloTriangleApplication::onQuit()
{
  auto* renderer = FlurrCore::Get().getRenderer();
  renderer->destroyVertexArray(m_va1Handle);
  renderer->destroyVertexBuffer(m_vb1PosHandle);
  renderer->destroyVertexBuffer(m_vb1ColorHandle);
  renderer->destroyVertexBuffer(m_ib1Handle);
  renderer->destroyShaderProgram(m_sp1Handle);
}

} // namespace flurr
