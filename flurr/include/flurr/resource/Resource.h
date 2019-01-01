#pragma once

#include "flurr/FlurrDefines.h"

#include <atomic>
#include <mutex>
#include <string>

namespace flurr
{

class ResourceManager;

enum class ResourceType : uint8_t
{
  kShader = 0
};

enum class ResourceState : uint8_t
{
  kCreated = 0,
  kLoading,
  kLoaded,
  kUnloading,
  kDestroying
};

class FLURR_DLL_EXPORT Resource
{

  friend class ResourceManager;

protected:

  Resource(FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex, ResourceManager* a_owningManager);
  Resource(const Resource&) = delete;
  Resource(Resource&&) = delete;
  Resource& operator=(const Resource&) = delete;
  Resource& operator=(Resource&&) = delete;

public:

  virtual ~Resource() = default; // needed so unique_ptr can delete Resource objects

  virtual ResourceType getResourceType() const = 0;
  FlurrHandle getResourceHandle() const { return m_resourceHandle; }
  const std::string& getResourcePath() const { return m_resourcePath; }
  std::size_t getResourceDirectoryIndex() const { return m_resourceDirectoryIndex; }
  std::string getResourceFullPath() const;
  ResourceManager* getOwningManager() const { return m_owningManager; }
  ResourceState getResourceState() const { return m_resourceState; }
  std::unique_lock<std::mutex> lockResource() { return std::unique_lock<std::mutex>(m_resourceMutex); }

private:

  void setResourceState(ResourceState a_state) { m_resourceState = a_state; }

  virtual Status onLoad(const std::string& a_fullPath) = 0;
  virtual void onUnload() = 0;
  virtual void onDestroy() = 0; // destroy any child resources created by onLoad

  FlurrHandle m_resourceHandle;
  std::string m_resourcePath;
  std::size_t m_resourceDirectoryIndex;
  ResourceManager* m_owningManager;
  std::atomic<ResourceState> m_resourceState;

  std::mutex m_resourceMutex;
};

} // namespace flurr
