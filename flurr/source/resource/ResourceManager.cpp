#include "flurr/resource/ResourceManager.h"
#include "flurr/resource/ShaderResource.h"
#include "flurr/resource/TextureResource.h"
#include "flurr/FlurrLog.h"

#include <algorithm>
#include <chrono>
#include <cstdio>

namespace flurr
{

ResourceManager::ResourceManager()
  : m_running(false),
  m_stopResourceThread(false),
  m_nextResourceHandle(1)
{
}

ResourceManager::~ResourceManager()
{
  if (isRunning())
    stop();
}

bool ResourceManager::run()
{
  if (isRunning())
  {
    FLURR_LOG_WARN("ResourceManager is already running!");
    return true;
  }

  m_resourceThread = std::thread(&ResourceManager::resourceThread, this);
  return true;
}

void ResourceManager::stop()
{
  destroyAllResources();

  // Remove any remaining resource listeners
  std::unique_lock<std::mutex> listenerLock(m_resourceListenerMutex);
  m_resourceListeners.clear();
  listenerLock.unlock();

  m_stopResourceThread = true;
  if (m_resourceThread.joinable())
    m_resourceThread.join();
}

void ResourceManager::updateListeners()
{
  if (!isRunning())
  {
    FLURR_LOG_WARN("Don't update listeners when ResourceManager is not running!");
    return;
  }

  // Notify listeners about resource operation results
  std::unique_lock<std::mutex> operationResultLock(m_resourceListenerMutex);
  if (!m_resourceOperationResultQueue.empty())
  {
    // Get operations result queue
    std::swap(m_resourceOperationResultQueue, m_activeResourceOperationResults);
    operationResultLock.unlock();

    // Notify listeners about pending results
    for (const auto& result : m_activeResourceOperationResults)
      for (auto* listener : m_resourceListeners)
        listener->onResourceOperation(result, this);
    m_activeResourceOperationResults.clear();
  }
  else
  {
    operationResultLock.unlock();
  }
}

Status ResourceManager::createResource(ResourceType a_resourceType, const std::string& a_resourcePath, FlurrHandle& a_resourceHandle)
{
  a_resourceHandle = INVALID_HANDLE; // in case resource creation fails

  if (!isRunning())
  {
    FLURR_LOG_WARN("Unable to create resource %s; ResourceManager not running!", a_resourcePath.c_str());
    return Status::kInvalidState;
  }

  // Try to create resource
  std::lock_guard<std::mutex> resourceLock(m_resourceMutex);
  const auto&& resourceHandleIt = m_resourceHandlesByPath.find(a_resourcePath);
  Resource* resource = resourceHandleIt != m_resourceHandlesByPath.end() ? m_resources.at(resourceHandleIt->second).get() : nullptr;
  if (resource)
  {
    FLURR_LOG_ERROR("Unable to create resource %s; resource already exists!", a_resourcePath.c_str());
    return Status::kResourceAlreadyExists;
  }
  else
  {
    // Try to resolve resource file
    auto&& findResourceResult = findResource(a_resourcePath);
    if (findResourceResult.resourceFound)
    {
      // Resource file found, create resource
      auto* resource = createResourceOfType(a_resourceType, m_nextResourceHandle, a_resourcePath, findResourceResult.resourceDirectoryIndex);
      m_resources[resource->getResourceHandle()] = std::unique_ptr<Resource>(resource);
      m_resourceHandlesByPath[a_resourcePath] = resource->getResourceHandle();
      resource->setResourceState(ResourceState::kCreated);
      ++m_nextResourceHandle;

      FLURR_LOG_DEBUG("Resource %s created successfully (resolved at %s).", a_resourcePath.c_str(), findResourceResult.fullResourcePath.c_str());
      a_resourceHandle = resource->getResourceHandle();
      return Status::kSuccess;
    }
    else
    {
      FLURR_LOG_ERROR("Unable to create resource %s; failed to resolve file!", a_resourcePath.c_str());
      return Status::kResourceFileNotFound;
    }
  }
}

Status ResourceManager::loadResource(FlurrHandle a_resourceHandle, bool a_loadAsyc)
{
  if (!isRunning())
  {
    FLURR_LOG_WARN("Unable to load resource %u; ResourceManager not running!", a_resourceHandle);
    return Status::kInvalidState;
  }

  if (INVALID_HANDLE == a_resourceHandle)
  {
    FLURR_LOG_ERROR("Unable to load resource %u; invalid handle!", a_resourceHandle);
    return Status::kInvalidHandle;
  }

  if (a_loadAsyc)
  {
    // Load resource asynchronousy (on the resource thread)
    std::lock_guard<std::mutex> resourceOperationLock(m_resourceOperationMutex);
    m_resourceOperationQueue.emplace_back(ResourceOperation{ResourceOperationType::kLoad, a_resourceHandle});
    return Status::kSuccess;
  }
  else
  {
    // Load resource right away (on the current thread)
    return executeLoadResource(a_resourceHandle).status;
  }
}

Status ResourceManager::unloadResource(FlurrHandle a_resourceHandle)
{
  if (!isRunning())
  {
    FLURR_LOG_WARN("Unable to unload resource %u; ResourceManager not running!", a_resourceHandle);
    return Status::kInvalidState;
  }

  return executeUnloadResource(a_resourceHandle).status;
}

Status ResourceManager::destroyResource(FlurrHandle a_resourceHandle)
{
  if (!isRunning())
  {
    FLURR_LOG_WARN("Unable to destroy resource %u; ResourceManager not running!", a_resourceHandle);
    return Status::kInvalidState;
  }

  // Try to destroy the resource
  std::unique_lock<std::mutex> allResourcesLock(m_resourceMutex);
  const auto&& resourceIt = m_resources.find(a_resourceHandle);
  Resource* resource = resourceIt != m_resources.end() ? resourceIt->second.get() : nullptr;
  if (resource)
  {
    // Mark resource as Destroying to prevent it from getting loaded/unloaded on another thread
    auto&& resourceLock = resource->lockResource();
    const bool mustUnload = resource->getResourceState() == ResourceState::kLoaded;
    resource->setResourceState(ResourceState::kDestroying);
    allResourcesLock.unlock(); // this is OK, resource can't get destroyed while locked

    // Unload the resource
    if (mustUnload)
      resource->onUnload();
    resource->onDestroy(); // destroy any child resources
    resourceLock.unlock();

    // Destroy the resource
    allResourcesLock.lock();
    const std::string resourcePath = resource->getResourcePath();
    m_resourceHandlesByPath.erase(resourcePath);
    m_resources.erase(a_resourceHandle);
    allResourcesLock.unlock();

    FLURR_LOG_DEBUG("Resource %s destroyed successfully.", resourcePath.c_str());
    return Status::kSuccess;
  }
  else
  {
    FLURR_LOG_ERROR("Unable to destroy resource %u; resource does not exist!", a_resourceHandle);
    return Status::kResourceNotCreated;
  }
}

Status ResourceManager::destroyAllResources()
{
  if (!isRunning())
  {
    FLURR_LOG_WARN("Unable to destroy all resources; ResourceManager not running!");
    return Status::kInvalidState;
  }

  // Get list of resources to destroy
  std::unique_lock<std::mutex> resourceLock(m_resourceMutex);
  std::vector<FlurrHandle> resourcesToDestroy;
  for (auto&& resourceKvp : m_resources)
    resourcesToDestroy.push_back(resourceKvp.first);
  resourceLock.unlock();

  // Destroy the resources
  Status result = Status::kSuccess;
  for (const auto resourceHandle : resourcesToDestroy)
  {
    result = destroyResource(resourceHandle);
    if (Status::kSuccess != result)
      return result;
  }

  return result;
}

bool ResourceManager::hasResource(FlurrHandle a_resourceHandle) const
{
  std::lock_guard<std::mutex> resourceLock(m_resourceMutex);
  return m_resources.find(a_resourceHandle) != m_resources.end();
}

bool ResourceManager::hasResource(const std::string& a_resourcePath) const
{
  std::lock_guard<std::mutex> resourceLock(m_resourceMutex);
  const auto&& resourceHandleIt = m_resourceHandlesByPath.find(a_resourcePath);
  return resourceHandleIt != m_resourceHandlesByPath.end();
}

Resource* ResourceManager::getResource(FlurrHandle a_resourceHandle) const
{
  std::lock_guard<std::mutex> resourceLock(m_resourceMutex);
  const auto&& resourceIt = m_resources.find(a_resourceHandle);
  return (resourceIt != m_resources.end()) ?
    resourceIt->second.get() :
    nullptr;
}

Resource* ResourceManager::getResource(const std::string& a_resourcePath) const
{
  std::lock_guard<std::mutex> resourceLock(m_resourceMutex);
  const auto&& resourceHandleIt = m_resourceHandlesByPath.find(a_resourcePath);
  return (resourceHandleIt != m_resourceHandlesByPath.end()) ?
    m_resources.find(resourceHandleIt->second)->second.get() :
    nullptr;
}

void ResourceManager::addListener(ResourceListener* a_listener)
{
  std::lock_guard<std::mutex> listenerLock(m_resourceListenerMutex);
  if (std::find(m_resourceListeners.begin(), m_resourceListeners.end(), a_listener) != m_resourceListeners.end())
    return;
  m_resourceListeners.push_back(a_listener);
}

void ResourceManager::removeListener(ResourceListener* a_listener)
{
  std::lock_guard<std::mutex> listenerLock(m_resourceListenerMutex);
  m_resourceListeners.erase(std::remove(m_resourceListeners.begin(), m_resourceListeners.end(), a_listener), m_resourceListeners.end());
}

Status ResourceManager::addResourceDirectory(const std::string& a_directory)
{
  if (isRunning())
  {
    FLURR_LOG_WARN("Unable to add resource directory %s; ResourceManager already running!", a_directory.c_str());
    return Status::kInvalidState;
  }

  std::string directory =
    (a_directory.length() > 0 && a_directory[a_directory.length()-1] != '/' && a_directory[a_directory.length()-1] != '\\') ?
    (a_directory + "/") :
    a_directory;

  if (std::find(m_resourceDirectories.begin(), m_resourceDirectories.end(), directory) != m_resourceDirectories.end())
    return Status::kSuccess;
  m_resourceDirectories.emplace_back(directory);
  return Status::kSuccess;
}

Status ResourceManager::removeResourceDirectory(std::size_t a_directoryIndex)
{
  if (isRunning())
  {
    FLURR_LOG_WARN("Unable to remove resource directory %u; ResourceManager already running!", a_directoryIndex);
    return Status::kInvalidState;
  }

  if (a_directoryIndex >= m_resourceDirectories.size())
  {
    FLURR_LOG_WARN("Unable to remove resource directory %u; index out of bounds!", a_directoryIndex);
    return Status::kIndexOutOfBounds;
  }
  m_resourceDirectories.erase(m_resourceDirectories.begin() + a_directoryIndex);
  return Status::kSuccess;
}

Status ResourceManager::removeAllResourceDirectories()
{
  if (isRunning())
  {
    FLURR_LOG_WARN("Unable to remove all resource directories; ResourceManager already running!");
    return Status::kInvalidState;
  }

  m_resourceDirectories.clear();
  return Status::kSuccess;
}

std::string ResourceManager::getResourceDirectory(std::size_t a_directoryIndex) const
{
 return a_directoryIndex >= m_resourceDirectories.size() ? "" : m_resourceDirectories[a_directoryIndex];
}

std::size_t ResourceManager::getResourceDirectoryCount() const
{
  return m_resourceDirectories.size();
}

FindResourceResult ResourceManager::findResource(const std::string& a_resourcePath) const
{
  for (std::size_t pathIndex = 0; pathIndex < m_resourceDirectories.size(); ++pathIndex)
  {
    std::string fullPath = m_resourceDirectories[pathIndex] + a_resourcePath;
    FILE* resourceFile = nullptr;
    fopen_s(&resourceFile, fullPath.c_str(), "rb");
    if (resourceFile)
    {
      return FindResourceResult{true, fullPath, pathIndex};
    }
  }

  return FindResourceResult{false, "", 0};
}

void ResourceManager::resourceThread()
{
  m_running = true;

  while (m_running)
  {
    if (m_stopResourceThread) {
      // This will be the last run of the resource thread
      // that will execute any pending resource operations
      m_running = false;
      m_stopResourceThread = false;
    }

    // Execute resource operations
    std::unique_lock<std::mutex> operationLock(m_resourceOperationMutex);
    if (!m_resourceOperationQueue.empty())
    {
      // Get operations queue
      std::swap(m_resourceOperationQueue, m_activeResourceOperations);
      operationLock.unlock();

      // Execute enqueued operations
      for (const auto& operation : m_activeResourceOperations)
      {
        auto&& operationResult = executeResourceOperation(operation);

        // Add operation result to queue
        std::lock_guard<std::mutex> operationResultLock(m_resourceListenerMutex);
        m_resourceOperationResultQueue.emplace_back(operationResult);
      }
      m_activeResourceOperations.clear();
    }
    else
    {
      operationLock.unlock();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(kResourceThreadSleepTime));
  }

  // Force-delete any remaining resources
  std::lock_guard<std::mutex> resourceLock(m_resourceMutex);
  m_nextResourceHandle = 1;
  m_resources.clear();
  m_resourceHandlesByPath.clear();
}

ResourceOperationResult ResourceManager::executeResourceOperation(const ResourceOperation& a_operation)
{
  switch (a_operation.operationType)
  {
    case ResourceOperationType::kLoad:
    {
      return executeLoadResource(a_operation.resourceHandle);
    }
    case ResourceOperationType::kUnload:
    {
      return executeUnloadResource(a_operation.resourceHandle);
    }
    default:
    {
      FLURR_ASSERT(false, "Unhandled resource operation!");
    }
  }

  return ResourceOperationResult{INVALID_HANDLE, a_operation.operationType, Status::kFailed};
}

ResourceOperationResult ResourceManager::executeLoadResource(FlurrHandle a_resourceHandle)
{
  // Initialize operation result
  ResourceOperationResult operationResult{
    a_resourceHandle,
    ResourceOperationType::kLoad,
    Status::kFailed
  };

  // Try to load resource
  std::unique_lock<std::mutex> allResourcesLock(m_resourceMutex);
  const auto&& resourceIt = m_resources.find(a_resourceHandle);
  Resource* resource = resourceIt != m_resources.end() ? resourceIt->second.get() : nullptr;
  if (resource)
  {
    auto&& resourceLock = resource->lockResource();
    if (resource->getResourceState() == ResourceState::kLoaded)
    {
      FLURR_LOG_ERROR("Unable to load resource %s; resource already loaded or loading!", resource->getResourcePath().c_str());
      operationResult.status = Status::kResourceAlreadyLoaded;
    }
    else if (resource->getResourceState() == ResourceState::kDestroying)
    {
      FLURR_LOG_ERROR("Unable to load resource %s; resource being destroyed!", resource->getResourcePath().c_str());
      operationResult.status = Status::kResourceDestroying;
    }
    else
    {
      // Load the resource
      resource->setResourceState(ResourceState::kLoading);
      allResourcesLock.unlock(); // this is OK, resource can't get destroyed while locked
      auto&& resourceFullPath = resource->getResourceFullPath();
      operationResult.status = resource->onLoad(resourceFullPath);
      if (operationResult.status == Status::kSuccess)
        resource->setResourceState(ResourceState::kLoaded);
      else
        resource->setResourceState(ResourceState::kCreated);
    }
  }
  else
  {
    FLURR_LOG_ERROR("Unable to load resource %s; resource does not exist!", resource->getResourcePath().c_str());
    allResourcesLock.unlock();
    operationResult.status = Status::kResourceNotCreated;
  }

  return operationResult;
}

ResourceOperationResult ResourceManager::executeUnloadResource(FlurrHandle a_resourceHandle)
{
  // Initialize operation result
  ResourceOperationResult operationResult{
    a_resourceHandle,
    ResourceOperationType::kUnload,
    Status::kFailed
  };

  // Try to unload resource
  std::unique_lock<std::mutex> allResourcesLock(m_resourceMutex);
  const auto&& resourceIt = m_resources.find(a_resourceHandle);
  Resource* resource = resourceIt != m_resources.end() ? resourceIt->second.get() : nullptr;
  if (resource)
  {
    auto&& resourceLock = resource->lockResource();
    if (resource->getResourceState() == ResourceState::kCreated)
    {
      FLURR_LOG_ERROR("Unable to unload resource %s; resource not loaded!", resource->getResourcePath().c_str());
      operationResult.status = Status::kResourceNotLoaded;
    }
    else if (resource->getResourceState() == ResourceState::kDestroying)
    {
      FLURR_LOG_ERROR("Unable to unload resource %s; resource being destroyed!", resource->getResourcePath().c_str());
      operationResult.status = Status::kResourceDestroying;
    }
    else
    {
      // Unload the resource
      resource->setResourceState(ResourceState::kUnloading);
      allResourcesLock.unlock(); // this is OK, resource can't get destroyed while locked
      operationResult.status = Status::kSuccess;
      resource->onUnload();
      resource->setResourceState(ResourceState::kCreated);
      resourceLock.unlock();
    }
  }
  else
  {
    FLURR_LOG_ERROR("Unable to unload resource %u; resource does no exist!", a_resourceHandle);
    operationResult.status = Status::kResourceNotCreated;
  }

  return operationResult;
}

Resource* ResourceManager::createResourceOfType(ResourceType a_resourceType, FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex)
{
  switch (a_resourceType)
  {
    case ResourceType::kShader:
    {
      return new ShaderResource(a_resourceHandle, a_resourcePath, a_resourceDirectoryIndex, this);
    }
    case ResourceType::kTexture:
    {
      return new TextureResource(a_resourceHandle, a_resourcePath, a_resourceDirectoryIndex, this);
    }
    default:
    {
      FLURR_ASSERT(false, "Unhandled resource type!");
    }
  }

  return nullptr;
}

} // namespace flurr
