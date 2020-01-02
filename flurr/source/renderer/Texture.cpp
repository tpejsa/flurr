#include "flurr/renderer/Texture.h"
#include "flurr/FlurrLog.h"
#include "flurr/FlurrCore.h"

namespace flurr
{

Texture::Texture(FlurrHandle a_texHandle)
  : m_texHandle(a_texHandle),
  m_texResourceHandle(INVALID_HANDLE),
  m_texWrapMode(TextureWrapMode::kRepeat),
  m_texMinFilterMode(TextureMinFilterMode::kLinearMipmapLinear),
  m_texMagFilterMode(TextureMagFilterMode::kLinear)
{
}

std::unique_lock<std::mutex> Texture::acquireTextureResource(TextureResource** a_texResource) const
{
  // Try to get a pointer to the resource
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  Resource* resource = nullptr;
  auto&& resourceLock = resourceManager->acquireResource(&resource, m_texResourceHandle);
  if (!resource)
  {
    FLURR_LOG_ERROR("Texture resource %u does not exist!", m_texResourceHandle);
    *a_texResource = nullptr;
    return std::move(resourceLock);
  }

  // Make sure it's a texture resource
  if (ResourceType::kTexture != resource->getResourceType())
  {
    FLURR_LOG_ERROR("Resource %u is not a texture resource!", m_texResourceHandle);
    *a_texResource = nullptr;
    return std::move(resourceLock);
  }

  *a_texResource = static_cast<TextureResource*>(resource);
  return std::move(resourceLock);
}

TextureResource* Texture::getTextureResource() const
{
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  return static_cast<TextureResource*>(resourceManager->getResource(m_texResourceHandle));
}

Status Texture::initTexture(FlurrHandle a_texResourceHandle, TextureWrapMode a_texWrapMode, TextureMinFilterMode a_texMinFilterMode, TextureMagFilterMode a_texMagFilterMode)
{
  // Ensure we have a valid and loaded texture resource
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  Resource* resource = nullptr;
  auto&& resourceLock = resourceManager->acquireResource(&resource, a_texResourceHandle);
  if (!resource)
  {
    FLURR_LOG_ERROR("Unable to initialize texture; texture resource %u does not exist!", a_texResourceHandle);
    return Status::kResourceNotCreated;
  }
  if (ResourceType::kTexture != resource->getResourceType())
  {
    FLURR_LOG_ERROR("Unable to initialize texture; resource %s is not a texture resource!", resource->getResourcePath().c_str());
    return Status::kResourceTypeInvalid;
  }
  if (ResourceState::kLoaded != resource->getResourceState())
  {
    FLURR_LOG_ERROR("Unable to initialize texture; texture resource %s not loaded!", resource->getResourcePath().c_str());
    return Status::kResourceNotLoaded;
  }
  TextureResource* texResource = static_cast<TextureResource*>(resource);

  // Initialize texture
  m_texResourceHandle = a_texResourceHandle;
  m_texWrapMode = a_texWrapMode;
  m_texMinFilterMode = a_texMinFilterMode;
  m_texMagFilterMode = a_texMagFilterMode;
  const auto result = onInitTexture();
  resourceLock.unlock();

  return result;
}

void Texture::destroyTexture()
{
  onDestroyTexture();
  m_texResourceHandle = INVALID_HANDLE;
}

Status Texture::useTexture(TextureUnitIndex a_texUnit)
{
  if (INVALID_HANDLE == m_texResourceHandle)
  {
    FLURR_LOG_ERROR("Unable to use texture; not created yet!");
    return Status::kInvalidState;
  }

  if (a_texUnit > MAX_TEXTURE_UNIT)
  {
    FLURR_LOG_ERROR("Texture unit index out of bounds!");
    return Status::kIndexOutOfBounds;
  }

  return onUseTexture(a_texUnit);
}

} // namespace flurr
