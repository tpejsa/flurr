#pragma once

#include "flurr/FlurrDefines.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace flurr
{

constexpr FlurrHandle ROOT_NODE_HANDLE = 1;
constexpr char* const ROOT_NODE_NAME = "Root";
constexpr char* const NODE_NAME_PREFIX = "Node";

enum class NodeComponentType
{
  kCamera = 0,
  kLight,
  kModel
};

struct NodeComponentInitArgs
{
  virtual NodeComponentType componentType() const = 0;
};

class Node;
class NodeComponent;
class CameraComponent;

class FLURR_DLL_EXPORT SceneManager
{

public:

  SceneManager();
  SceneManager(const SceneManager&) = delete;
  SceneManager(SceneManager&&) = delete;
  SceneManager& operator=(const SceneManager&) = delete;
  SceneManager& operator=(SceneManager&&) = delete;
  ~SceneManager();

  bool isInitialized() const { return m_initialized; }
  Status init();
  void shutdown();
  Status update(float a_deltaTime);
  Status draw();

  Node* getRootNode() const { return getNode(ROOT_NODE_HANDLE); }
  Status createNode(FlurrHandle& a_nodeHandle, const std::string& a_nodeName = "", FlurrHandle a_parentNodeHandle = INVALID_HANDLE,
    const glm::vec3& a_position = glm::vec3(), const glm::quat& a_rotation = glm::quat(), const glm::vec3& a_scale = glm::vec3(1.0f));
  void destroyNode(FlurrHandle a_nodeHandle, bool a_destroyChildren = false);
  void destroyNode(const std::string& a_nodeName, bool a_destroyChildren = false);
  void destroyAllNodes();
  bool hasNode(FlurrHandle a_nodeHandle) const;
  bool hasNode(const std::string& a_nodeName) const;
  Node* getNode(FlurrHandle a_nodeHandle) const;
  Node* getNode(const std::string& a_nodeName) const;
  std::vector<FlurrHandle> getAllNodeHandles() const;
  Status createComponent(FlurrHandle& a_componentHandle, FlurrHandle a_nodeHandle, const NodeComponentInitArgs& a_initArgs);
  void destroyComponent(FlurrHandle a_componentHandle);
  void destroyAllComponents();
  void destroyAllComponentsOfType(NodeComponentType a_componentType);
  void destroyAllComponentsOfNode(FlurrHandle a_nodeHandle);
  bool hasComponent(FlurrHandle a_componentHandle) const;
  NodeComponent* getComponent(FlurrHandle a_componentHandle) const;
  std::vector<FlurrHandle> getAllComponentHandles() const;
  std::vector<FlurrHandle> getAllComponentHandlesOfType(NodeComponentType a_componentType) const;
  CameraComponent* getActiveCamera() const;
  void setActiveCameraHandle(FlurrHandle cameraHandle);

private:

  Status createNodeWithHandle(FlurrHandle a_nodeHandle, const std::string& a_nodeName, FlurrHandle a_parentNodeHandle,
    const glm::vec3& a_position = glm::vec3(), const glm::quat& a_rotation = glm::quat(), const glm::vec3& a_scale = glm::vec3(1.0f));
  void destroyEmptyNode(FlurrHandle a_nodeHandle);
  void removeComponent(FlurrHandle a_componentHandle);
  std::string generateNodeName();
  NodeComponent* createComponentOfType(FlurrHandle a_componentHandle, FlurrHandle a_nodeHandle, NodeComponentType a_componentType);

  bool m_initialized;

  // Nodes
  FlurrHandle m_nextNodeHandle;
  uint32_t m_nextNodeNameIndex;
  std::unordered_map<FlurrHandle, std::unique_ptr<Node>> m_nodes;
  std::unordered_map<std::string, FlurrHandle> m_nodeHandlesByName;
  // Node components
  FlurrHandle m_nextComponentHandle;
  std::unordered_map<FlurrHandle, std::unique_ptr<NodeComponent>> m_components;
  std::unordered_multimap<NodeComponentType, FlurrHandle> m_componentHandlesByType;
  // Rendering
  FlurrHandle m_activeCameraHandle;
};

} // namespace flurr
