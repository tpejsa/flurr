#include "HelloTexturesApplication.h"

#include <cmath>
#include <glm/gtc/constants.hpp>

namespace flurr
{

HelloTexturesApplication::HelloTexturesApplication(int a_windowWidth, int a_windowHeight)
  : FlurrApplication(a_windowWidth, a_windowHeight, "HelloTextures"),
  m_vsResourceHandle(INVALID_HANDLE),
  m_fsResourceHandle(INVALID_HANDLE),
  m_spHandle(INVALID_HANDLE),
  m_tex1ResourceHandle(INVALID_HANDLE),
  m_tex2ResourceHandle(INVALID_HANDLE),
  m_vb1PosHandle(INVALID_HANDLE),
  m_vb1UV0Handle(INVALID_HANDLE),
  m_ib1Handle(INVALID_HANDLE),
  m_va1Handle(INVALID_HANDLE),
  m_vb2PosHandle(INVALID_HANDLE),
  m_vb2UV0Handle(INVALID_HANDLE),
  m_ib2Handle(INVALID_HANDLE),
  m_va2Handle(INVALID_HANDLE)
{
}

bool HelloTexturesApplication::onInit()
{
  // Load Phong shader resources
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  if (Status::kSuccess != resourceManager->createResource(ResourceType::kShader, kVSPath, m_vsResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to create shader resource %s!", kVSPath);
    return false;
  }
  if (Status::kSuccess != resourceManager->loadResource(m_vsResourceHandle, false))
  {
    FLURR_LOG_ERROR("Failed to load shader resource %s!", kVSPath);
    return false;
  }
  if (Status::kSuccess != resourceManager->createResource(ResourceType::kShader, kFSPath, m_fsResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to create shader resource %s!", kFSPath);
    return false;
  }
  if (Status::kSuccess != resourceManager->loadResource(m_fsResourceHandle, false))
  {
    FLURR_LOG_ERROR("Failed to load shader resource %s!", kFSPath);
    return false;
  }

  // Compile and link Phong shaders
  auto* renderer = FlurrCore::Get().getRenderer();
  m_spHandle = renderer->createShaderProgram();
  if (Status::kSuccess != renderer->compileShader(m_spHandle, ShaderType::kVertex, m_vsResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to compile vertex shader %s!", kVSPath);
    return false;
  }
  if (Status::kSuccess != renderer->compileShader(m_spHandle, ShaderType::kFragment, m_fsResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to compile fragment shader %s!", kFSPath);
    return false;
  }
  if (Status::kSuccess != renderer->linkShaderProgram(m_spHandle))
  {
    FLURR_LOG_ERROR("Failed to link shader program 2!");
    return false;
  }

  // Load texture resources
  if (Status::kSuccess != resourceManager->createResource(ResourceType::kTexture, kTex1Path, m_tex1ResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to create texture resource %s!", kTex1Path);
    return false;
  }
  if (Status::kSuccess != resourceManager->createResource(ResourceType::kTexture, kTex2Path, m_tex2ResourceHandle))
  {
    FLURR_LOG_ERROR("Failed to create texture resource %s!", kTex2Path);
    return false;
  }

  // Create vertex buffers for geometry 1
  m_vb1PosHandle = renderer->createVertexBuffer();
  if (Status::kSuccess != renderer->initVertexBuffer(m_vb1PosHandle, VertexBufferType::kVertexAttribute,
    sizeof(kPositionData1), const_cast<float*>(&kPositionData1[0]), 3*sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for positions 1!");
    return false;
  }
  m_vb1UV0Handle = renderer->createVertexBuffer();
  if (Status::kSuccess != renderer->initVertexBuffer(m_vb1UV0Handle, VertexBufferType::kVertexAttribute,
    sizeof(kUV0Data1), const_cast<float*>(&kUV0Data1[0]), 2*sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for uv0 1!");
    return false;
  }
  m_ib1Handle = renderer->createVertexBuffer();
  if (Status::kSuccess != renderer->initIndexBuffer(m_ib1Handle,
    sizeof(kIndexData1), const_cast<uint32_t*>(&kIndexData1[0])))
  {
    FLURR_LOG_ERROR("Failed to create index buffer 1!");
    return false;
  }

  // Create vertex array for geometry 1
  m_va1Handle = renderer->createVertexArray();
  if (Status::kSuccess != renderer->initVertexArray(m_va1Handle, {m_vb1PosHandle, m_vb1UV0Handle}, m_ib1Handle))
  {
    FLURR_LOG_ERROR("Failed to create vertex array 1!");
    return false;
  }

  // Create vertex buffers for geometry 2
  m_vb2PosHandle = renderer->createVertexBuffer();
  if (Status::kSuccess != renderer->initVertexBuffer(m_vb2PosHandle, VertexBufferType::kVertexAttribute,
    sizeof(kPositionData2), const_cast<float*>(&kPositionData2[0]), 3*sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for positions 2!");
    return false;
  }
  m_vb2UV0Handle = renderer->createVertexBuffer();
  if (Status::kSuccess != renderer->initVertexBuffer(m_vb2UV0Handle, VertexBufferType::kVertexAttribute,
    sizeof(kUV0Data2), const_cast<float*>(&kUV0Data2[0]), 2*sizeof(float)))
  {
    FLURR_LOG_ERROR("Failed to create vertex buffer for uv0 2!");
    return false;
  }
  m_ib2Handle = renderer->createVertexBuffer();
  if (Status::kSuccess != renderer->initIndexBuffer(m_ib2Handle,
    sizeof(kIndexData2), const_cast<uint32_t*>(&kIndexData2[0])))
  {
    FLURR_LOG_ERROR("Failed to create index buffer 2!");
    return false;
  }

  // Create vertex array for geometry 2
  m_va2Handle = renderer->createVertexArray();
  if (Status::kSuccess != renderer->initVertexArray(m_va2Handle, {m_vb2PosHandle, m_vb2UV0Handle}, m_ib2Handle))
  {
    FLURR_LOG_ERROR("Failed to create vertex array 2!");
    return false;
  }

  // Safe to unload resources now that everything has been created
  resourceManager->unloadResource(m_vsResourceHandle);
  resourceManager->unloadResource(m_fsResourceHandle);

  return true;
}

bool HelloTexturesApplication::onUpdate(float a_deltaTime)
{
  return true;
}

void HelloTexturesApplication::onDraw()
{
  // Draw geometry 1
  auto* renderer = FlurrCore::Get().getRenderer();
  renderer->useShaderProgram(m_spHandle);
  renderer->drawVertexArray(m_va1Handle);

  // Draw geometry 2
  renderer->drawVertexArray(m_va2Handle);
}

void HelloTexturesApplication::onQuit()
{
  // Destroy geometry and shaders
  auto* renderer = FlurrCore::Get().getRenderer();
  renderer->destroyVertexArray(m_va1Handle);
  renderer->destroyVertexBuffer(m_vb1PosHandle);
  renderer->destroyVertexBuffer(m_vb1UV0Handle);
  renderer->destroyVertexBuffer(m_ib1Handle);
  renderer->destroyVertexArray(m_va2Handle);
  renderer->destroyVertexBuffer(m_vb2PosHandle);
  renderer->destroyVertexBuffer(m_vb2UV0Handle);
  renderer->destroyVertexBuffer(m_ib2Handle);
  renderer->destroyShaderProgram(m_spHandle);

  // Clean up resources
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  resourceManager->destroyAllResources();
}

} // namespace flurr
