#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/resource/Resource.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace flurr
{

struct FindResourceResult
{
  bool resourceFound;
  std::string fullResourcePath;
  std::size_t resourceDirectoryIndex;
};

enum class ResourceOperationType : uint8_t
{
  kLoad = 0,
  kUnload
};

struct ResourceOperation
{
  ResourceOperationType operationType;
  FlurrHandle resourceHandle;
};

struct ResourceOperationResult
{
  FlurrHandle resourceHandle;
  ResourceOperationType operationType;
  Status status;
};

class ResourceListener
{

public:

  virtual void onResourceOperation(const ResourceOperationResult& a_result, ResourceManager* a_resourceManager) = 0;
};

class FLURR_DLL_EXPORT ResourceManager
{

public:

  ResourceManager();
  ResourceManager(const ResourceManager&) = delete;
  ResourceManager(ResourceManager&&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;
  ResourceManager& operator=(ResourceManager&&) = delete;
  virtual ~ResourceManager();

  bool run();
  bool isRunning() const { return m_running; }
  void stop();
  void updateListeners();

  Status createResource(FlurrHandle& a_resourceHandle, ResourceType a_resourceType, const std::string& a_resourcePath);
  Status loadResource(FlurrHandle a_resourceHandle, bool a_loadAsyc = true);
  Status unloadResource(FlurrHandle a_resourceHandle);
  Status destroyResource(FlurrHandle a_resourceHandle);
  Status destroyAllResources();

  bool hasResource(FlurrHandle a_resourceHandle) const;
  bool hasResource(const std::string& a_resourcePath) const;
  std::unique_lock<std::mutex> acquireResource(Resource** a_resource, FlurrHandle a_resourceHandle);
  std::unique_lock<std::mutex> acquireResource(Resource** a_resource, const std::string& a_resourcePath);
  std::unique_lock<std::mutex> lockResources() { return std::unique_lock<std::mutex>(m_resourceMutex); }
  Resource* getResource(FlurrHandle a_resourceHandle) const; // not thread-safe; call lockResources beforehand
  Resource* getResource(const std::string& a_resourcePath) const; // not thread-safe; call lockResources beforehand

  void addListener(ResourceListener* a_listener);
  void removeListener(ResourceListener* a_listener);

  Status addResourceDirectory(const std::string& a_directory);
  Status removeResourceDirectory(std::size_t a_directoryIndex);
  Status removeAllResourceDirectories();
  std::string getResourceDirectory(std::size_t a_directoryIndex) const;
  std::size_t getResourceDirectoryCount() const;

private:

  FindResourceResult findResource(const std::string& a_resourcePath) const;
  void resourceThread();
  ResourceOperationResult executeResourceOperation(const ResourceOperation& a_operation);
  ResourceOperationResult executeLoadResource(FlurrHandle a_resourceHandle);
  ResourceOperationResult executeUnloadResource(FlurrHandle a_resourceHandle);
  Resource* createResourceOfType(ResourceType a_resourceType, FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex);

  static constexpr int kResourceThreadSleepTime = 25;

  std::atomic_bool m_running;
  std::thread m_resourceThread;
  std::atomic_bool m_stopResourceThread;

  std::mutex m_resourceOperationMutex;
  std::vector<ResourceOperation> m_resourceOperationQueue;
  std::vector<ResourceOperation> m_activeResourceOperations;

  std::vector<ResourceListener*> m_resourceListeners;
  std::mutex m_resourceListenerMutex;
  std::vector<ResourceOperationResult> m_resourceOperationResultQueue;
  std::vector<ResourceOperationResult> m_activeResourceOperationResults;

  mutable std::mutex m_resourceMutex;
  std::vector<std::string> m_resourceDirectories;
  FlurrHandle m_nextResourceHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<Resource>> m_resources;
  std::unordered_map<std::string, FlurrHandle> m_resourceHandlesByPath;
};

} // namespace flurr
