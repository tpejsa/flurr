#include "HelloTriangleApplication.h"

#include <cmath>
#include <glm/gtc/constants.hpp>

namespace flurr
{

HelloTriangleApplication::HelloTriangleApplication(int a_windowWidth, int a_windowHeight)
  : FlurrApplication(a_windowWidth, a_windowHeight, "HelloTriangle"),
  m_vs1ResourceHandle(INVALID_HANDLE),
  m_fs1ResourceHandle(INVALID_HANDLE),
  m_vs2ResourceHandle(INVALID_HANDLE),
  m_fs2ResourceHandle(INVALID_HANDLE),
  m_sp1Handle(INVALID_HANDLE),
  m_sp2Handle(INVALID_HANDLE),
  m_vb1PosHandle(INVALID_HANDLE),
  m_vb1ColorHandle(INVALID_HANDLE),
  m_ib1Handle(INVALID_HANDLE),
  m_va1Handle(INVALID_HANDLE),
  m_vb2PosHandle(INVALID_HANDLE),
  m_ib2Handle(INVALID_HANDLE),
  m_va2Handle(INVALID_HANDLE),
  m_albedoTime(0.0f)
{
}

bool HelloTriangleApplication::onInit()
{
  // Load vertex color shader resources
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  if (Status::kSuccess != resourceManager->createResource(m_vs1ResourceHandle, ResourceType::kShader, kVS1Path))
  {
    FLURR_LOG_ERROR("Failed to create shader resource %s!", kVS1Path);
    return false;
  }
  if (Status::kSuccess != resourceManager->loadResource(m_vs1ResourceHandle, false))
  {
    FLURR_LOG_ERROR("Failed to load shader resource %s!", kVS1Path);
    return false;
  }
  if (Status::kSuccess != resourceManager->createResource(m_fs1ResourceHandle, ResourceType::kShader, kFS1Path))
  {
    FLURR_LOG_ERROR("Failed to create shader resource %s!", kFS1Path);
    return false;
  }
  if (Status::kSuccess != resourceManager->loadResource(m_fs1ResourceHandle, false))
  {
    FLURR_LOG_ERROR("Failed to load shader resource %s!", kFS1Path);
    return false;
  }

  // Compile and link vertex color shaders
  auto* renderer = FlurrCore::Get().getRenderer();
  renderer->createShaderProgram(m_sp1Handle);
  if (Status::kSuccess != renderer->compileShader(m_sp1Handle, ShaderType::kVertex, m_vs1ResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to compile vertex shader %s!", kVS1Path);
    return false;
  }
  if (Status::kSuccess != renderer->compileShader(m_sp1Handle, ShaderType::kFragment, m_fs1ResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to compile fragment shader %s!", kFS1Path);
    return false;
  }
  if (Status::kSuccess != renderer->linkShaderProgram(m_sp1Handle))
  {
    FLURR_LOG_ERROR("Failed to link shader program 1!");
    return false;
  }

  // Create vertex buffers for geometry 1
  Status result = renderer->createVertexBuffer(m_vb1PosHandle, VertexBufferType::kVertexAttribute,
    sizeof(kPositionData1), const_cast<float*>(&kPositionData1[0]), 3*sizeof(float));
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for positions 1!");
    return false;
  }
  result = renderer->createVertexBuffer(m_vb1ColorHandle, VertexBufferType::kVertexAttribute,
    sizeof(kColorData1), const_cast<float*>(&kColorData1[0]), 3*sizeof(float));
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for colors 1!");
    return false;
  }
  result = renderer->createIndexBuffer(m_ib1Handle, sizeof(kIndexData1), const_cast<uint32_t*>(&kIndexData1[0]));
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create index buffer 1!");
    return false;
  }

  // Create vertex array for geometry 1
  result = renderer->createVertexArray(m_va1Handle, {m_vb1PosHandle, m_vb1ColorHandle}, m_ib1Handle);
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create vertex array 1!");
    return false;
  }

  // Load Phong shader resources
  if (Status::kSuccess != resourceManager->createResource(m_vs2ResourceHandle, ResourceType::kShader, kVS2Path))
  {
    FLURR_LOG_ERROR("Failed to create shader resource %s!", kVS2Path);
    return false;
  }
  if (Status::kSuccess != resourceManager->loadResource(m_vs2ResourceHandle, false))
  {
    FLURR_LOG_ERROR("Failed to load shader resource %s!", kVS2Path);
    return false;
  }
  if (Status::kSuccess != resourceManager->createResource(m_fs2ResourceHandle, ResourceType::kShader, kFS2Path))
  {
    FLURR_LOG_ERROR("Failed to create shader resource %s!", kFS2Path);
    return false;
  }
  if (Status::kSuccess != resourceManager->loadResource(m_fs2ResourceHandle, false))
  {
    FLURR_LOG_ERROR("Failed to load shader resource %s!", kFS2Path);
    return false;
  }

  // Compile and link Phong shaders
  result = renderer->createShaderProgram(m_sp2Handle);
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create shader program 2!");
    return false;
  }
  if (Status::kSuccess != renderer->compileShader(m_sp2Handle, ShaderType::kVertex, m_vs2ResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to compile vertex shader %s!", kVS2Path);
    return false;
  }
  if (Status::kSuccess != renderer->compileShader(m_sp2Handle, ShaderType::kFragment, m_fs2ResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to compile fragment shader %s!", kFS2Path);
    return false;
  }
  if (Status::kSuccess != renderer->linkShaderProgram(m_sp2Handle))
  {
    FLURR_LOG_ERROR("Failed to link shader program 2!");
    return false;
  }

  // Create vertex buffers for geometry 2
  result = renderer->createVertexBuffer(m_vb2PosHandle, VertexBufferType::kVertexAttribute,
    sizeof(kPositionData2), const_cast<float*>(&kPositionData2[0]), 3*sizeof(float));
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for positions 2!");
    return false;
  }
  result = renderer->createIndexBuffer(m_ib2Handle, sizeof(kIndexData2), const_cast<uint32_t*>(&kIndexData2[0]));
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create index buffer 2!");
    return false;
  }

  // Create vertex array for geometry 2
  result = renderer->createVertexArray(m_va2Handle, {m_vb2PosHandle}, m_ib2Handle);
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create vertex array 2!");
    return false;
  }

  // Safe to unload resources now that everything has been created
  resourceManager->unloadResource(m_vs1ResourceHandle);
  resourceManager->unloadResource(m_fs1ResourceHandle);
  resourceManager->unloadResource(m_vs2ResourceHandle);
  resourceManager->unloadResource(m_fs2ResourceHandle);

  // Disable diffuse texture map in the Phong shader
  renderer->useShaderProgram(m_sp2Handle);
  auto* sp2 = renderer->getShaderProgram(m_sp2Handle);
  if (!sp2->setBoolValue("diffuseMapEnabled", false))
    FLURR_LOG_ERROR("Failed to resolve uniform diffuseMapEnabled!");

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
  renderer->useShaderProgram(m_sp1Handle);
  renderer->drawVertexArray(m_va1Handle);

  // Draw geometry 2
  renderer->useShaderProgram(m_sp2Handle);
  auto* sp2 = renderer->getShaderProgram(m_sp2Handle);
  const glm::vec4 diffuseColor(
    sin(m_albedoTime*glm::pi<float>()/2.0f)/2.0f + 0.5f,
    sin((m_albedoTime/2.0f + 0.25f)*glm::pi<float>())/2.0f + 0.5f,
    sin((m_albedoTime/2.0f + 0.5f)*glm::pi<float>())/2.0f + 0.5f,
    1.0f);
  if (!sp2->setVec4Value("diffuseColor", diffuseColor))
    FLURR_LOG_ERROR("Failed to resolve uniform diffuseColor!");
  renderer->drawVertexArray(m_va2Handle);
}

void HelloTriangleApplication::onQuit()
{
  // Destroy geometry and shaders
  auto* renderer = FlurrCore::Get().getRenderer();
  renderer->destroyVertexArray(m_va1Handle);
  renderer->destroyVertexBuffer(m_vb1PosHandle);
  renderer->destroyVertexBuffer(m_vb1ColorHandle);
  renderer->destroyVertexBuffer(m_ib1Handle);
  renderer->destroyShaderProgram(m_sp1Handle);
  renderer->destroyVertexArray(m_va2Handle);
  renderer->destroyVertexBuffer(m_vb2PosHandle);
  renderer->destroyVertexBuffer(m_ib2Handle);
  renderer->destroyShaderProgram(m_sp2Handle);

  // Clean up resources
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  resourceManager->destroyAllResources();
}

} // namespace flurr
