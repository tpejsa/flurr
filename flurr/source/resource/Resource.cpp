#include "flurr/resource/Resource.h"
#include "flurr/resource/ResourceManager.h"
#include "flurr/FlurrLog.h"

namespace flurr
{

Resource::Resource(FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex, ResourceManager* a_owningManager)
  : m_resourceHandle(a_resourceHandle),
  m_resourcePath(a_resourcePath),
  m_resourceDirectoryIndex(a_resourceDirectoryIndex),
  m_owningManager(a_owningManager),
  m_resourceState(ResourceState::kCreated)
{
}

std::string Resource::getResourceFullPath() const
{
  FLURR_ASSERT(m_resourceDirectoryIndex < m_owningManager->getResourceDirectoryCount(), "Resource %s is pointing to non-existant resource directory %u!",
    m_resourcePath.c_str(), m_resourceDirectoryIndex);
  return m_owningManager->getResourceDirectory(m_resourceDirectoryIndex) + m_resourcePath;
}

} // namespace flurr
