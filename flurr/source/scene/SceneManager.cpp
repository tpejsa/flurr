#include "flurr/scene/SceneManager.h"
#include "flurr/scene/CameraComponent.h"
#include "flurr/scene/Node.h"
#include "flurr/scene/NodeComponent.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/TypeCasts.h"

#include <algorithm>

namespace flurr
{

SceneManager::SceneManager()
  : m_initialized(false),
  m_nextNodeHandle(ROOT_NODE_HANDLE + 1),
  m_nextNodeNameIndex(0),
  m_nextComponentHandle(1),
  m_activeCameraHandle(INVALID_HANDLE)
{
}

SceneManager::~SceneManager()
{
  if (isInitialized())
    shutdown();
}

Status SceneManager::init()
{
  FLURR_LOG_INFO("Initializing SceneManager...");
  if (isInitialized())
  {
    FLURR_LOG_WARN("SceneManager already initialized!");
    return Status::kSuccess;
  }

  // Create root node
  Status result = createNodeWithHandle(ROOT_NODE_HANDLE, ROOT_NODE_NAME, INVALID_HANDLE);
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to create root node!");
    return result;
  }

  FLURR_LOG_INFO("SceneManager initialized.");
  m_initialized = true;
  return Status::kSuccess;
}

void SceneManager::shutdown()
{
  FLURR_LOG_INFO("Shutting down SceneManager...");
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return;
  }

  // Delete all the nodes and components
  destroyAllNodes();
  destroyEmptyNode(ROOT_NODE_HANDLE);
  m_nextComponentHandle = 1;
  m_nextNodeHandle = ROOT_NODE_HANDLE + 1;
  m_nextNodeNameIndex = 0;

  m_initialized = false;
  FLURR_LOG_INFO("SceneManager shutdown complete.");
}

Status SceneManager::update(float a_deltaTime)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return Status::kNotInitialized;
  }

  return getRootNode()->updateNode(a_deltaTime);
}

Status SceneManager::draw()
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return Status::kNotInitialized;
  }

  // TODO: render the scene

  return Status::kSuccess;
}

Status SceneManager::createNode(FlurrHandle& a_nodeHandle, const std::string& a_nodeName, FlurrHandle a_parentNodeHandle,
  const glm::vec3& a_position, const glm::quat& a_rotation, const glm::vec3& a_scale)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return Status::kNotInitialized;
  }

  // Get the parent node handle
  FlurrHandle parentNodeHandle = ROOT_NODE_HANDLE;
  if (INVALID_HANDLE != a_parentNodeHandle)
  {
    if (hasNode(a_parentNodeHandle))
    {
      parentNodeHandle = a_parentNodeHandle;
    }
    else
    {
      FLURR_LOG_ERROR("Unable to create node %s; specified parent node %u does not exist!", a_nodeName.c_str(), a_parentNodeHandle);
      return Status::kInvalidHandle;
    }
  }

  // Get or generate the node name
  std::string nodeName;
  if (a_nodeName.empty())
  {
    nodeName = generateNodeName();
    if (nodeName.empty())
    {
      FLURR_LOG_ERROR("Failed to generate name for new node!");
      return Status::kFailed;
    }
  }
  else if (hasNode(a_nodeName))
  {
    FLURR_LOG_ERROR("Unable create node %s; duplicate node name!", a_nodeName.c_str());
    return Status::kInvalidArgument;
  }
  else
  {
    nodeName = a_nodeName;
  }

  // Generate node handle
  a_nodeHandle = GenerateHandle(m_nextNodeHandle, [this](FlurrHandle a_h) { return hasNode(a_h); });

  return createNodeWithHandle(a_nodeHandle, nodeName, parentNodeHandle, a_position, a_rotation, a_scale);
}

void SceneManager::destroyNode(FlurrHandle a_nodeHandle, bool a_destroyChildren)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return;
  }

  // Does the specified node exist?
  auto* node = getNode(a_nodeHandle);
  if (!node)
  {
    FLURR_LOG_WARN("Node %u does not exist!", a_nodeHandle);
    return;
  }
  auto nodeName = node->getNodeName();

  // Recursively destroy the specified node's children or reparent them to the root node
  const auto childNodeHandles = node->getAllChildNodeHandles();
  for (const auto childNodeHandle : childNodeHandles)
  {
    if (a_destroyChildren)
      destroyNode(childNodeHandle, a_destroyChildren);
    else
      getRootNode()->addChildNode(childNodeHandle);
  }
  
  // Delete the specified node's components
  auto componentHandles = node->getAllComponentHandles();
  node->removeAllComponents();
  for (const FlurrHandle componentHandle : componentHandles)
    destroyComponent(componentHandle);   

  destroyEmptyNode(a_nodeHandle);

  FLURR_LOG_INFO("Destroyed node %s (%u).", nodeName.c_str(), a_nodeHandle);
}

void SceneManager::destroyNode(const std::string& a_nodeName, bool a_destroyChildren)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return;
  }

  if (ROOT_NODE_NAME == a_nodeName)
  {
    FLURR_LOG_ERROR("Unable to destroy the root node!");
    return;
  }

  // Does the specified node exist?
  auto* node = getNode(a_nodeName);
  if (!node)
  {
    FLURR_LOG_WARN("Node %s does not exist!", a_nodeName.c_str());
    return;
  }

  destroyNode(node->getNodeHandle(), a_destroyChildren);
}

void SceneManager::destroyAllNodes()
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return;
  }

  const auto&& nodeHandles = getAllNodeHandles();
  for (const FlurrHandle nodeHandle : nodeHandles)
  {
    if (nodeHandle == ROOT_NODE_HANDLE)
      continue;
    destroyNode(nodeHandle);
  }

  FLURR_LOG_INFO("Destroyed all nodes.");
}

bool SceneManager::hasNode(FlurrHandle a_nodeHandle) const
{
  return m_nodes.find(a_nodeHandle) != m_nodes.end();
}

bool SceneManager::hasNode(const std::string& a_nodeName) const
{
  return m_nodeHandlesByName.find(a_nodeName) != m_nodeHandlesByName.end();
}

Node* SceneManager::getNode(FlurrHandle a_nodeHandle) const
{
  const auto nodeIt = m_nodes.find(a_nodeHandle);
  return nodeIt != m_nodes.end() ? nodeIt->second.get() : nullptr;
}

Node* SceneManager::getNode(const std::string& a_nodeName) const
{
  const auto nodeHandleIt = m_nodeHandlesByName.find(a_nodeName);
  return nodeHandleIt != m_nodeHandlesByName.end() ? getNode(nodeHandleIt->second) : nullptr;
}

std::vector<FlurrHandle> SceneManager::getAllNodeHandles() const
{
  std::vector<FlurrHandle> nodeHandles;
  nodeHandles.reserve(m_nodes.size());
  for (const auto& nodeKvp : m_nodes)
    nodeHandles.push_back(nodeKvp.first);

  return nodeHandles;
}

Status SceneManager::createComponent(FlurrHandle& a_componentHandle, FlurrHandle a_nodeHandle, const NodeComponentInitArgs& a_initArgs)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return Status::kNotInitialized;
  }

  // Get the node that will contain the component
  auto* node = getNode(a_nodeHandle);
  if (!node)
  {
    FLURR_LOG_ERROR("Unable to create component; node %u does not exist!", a_nodeHandle);
    return Status::kInvalidHandle;
  }

  if (node->isRootNode())
  {
    FLURR_LOG_ERROR("Unable to create a component on the root node!");
    return Status::kInvalidHandle;
  }

  // Generate component handle
  a_componentHandle = GenerateHandle(m_nextComponentHandle, [this](FlurrHandle a_h) { return hasComponent(a_h); });

  // Create component
  const NodeComponentType componentType = a_initArgs.componentType();
  auto* component = createComponentOfType(a_componentHandle, a_nodeHandle, componentType);
  m_components[a_componentHandle] = std::unique_ptr<NodeComponent>(component);
  m_componentHandlesByType.emplace(a_initArgs.componentType(), a_componentHandle);

  // Add component to node
  node->addComponent(a_componentHandle);

  FLURR_LOG_INFO("Created component of type %u on node %s (%u).", componentType,
    node->getNodeName().c_str(), node->getNodeHandle());

  // Initialize component
  Status result = component->initComponent(a_initArgs);
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to initialize component of type %u on node %s (%u).", componentType,
      node->getNodeName().c_str(), node->getNodeHandle());
    removeComponent(a_componentHandle);
    return result;
  }

  if (componentType == NodeComponentType::kCamera && m_activeCameraHandle == INVALID_HANDLE)
  {
    // There is no active camera yet, so set this newly created camera component to be our active camera
    setActiveCameraHandle(a_componentHandle);
  }

  return Status::kSuccess;
}

void SceneManager::destroyComponent(FlurrHandle a_componentHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return;
  }

  // Get the component
  auto* component = getComponent(a_componentHandle);
  if (!component)
  {
    FLURR_LOG_WARN("Node component %u does not exist!", a_componentHandle);
    return;
  }

  // If this component is the active camera, reset active camera
  if (a_componentHandle == m_activeCameraHandle)
  {
    setActiveCameraHandle(INVALID_HANDLE);
  }

  // Deinitialize the component
  component->destroyComponent();

  // Finish removing component from the scene
  removeComponent(a_componentHandle);
}

void SceneManager::destroyAllComponents()
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return;
  }

  auto componentHandles = getAllComponentHandles();
  for (const FlurrHandle componentHandle : componentHandles)
    destroyComponent(componentHandle);
}

void SceneManager::destroyAllComponentsOfType(NodeComponentType a_componentType)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return;
  }

  auto componentHandles = getAllComponentHandlesOfType(a_componentType);
  for (const FlurrHandle componentHandle : componentHandles)
    destroyComponent(componentHandle);
}

void SceneManager::destroyAllComponentsOfNode(FlurrHandle a_nodeHandle)
{
  if (!isInitialized())
  {
    FLURR_LOG_WARN("SceneManager not initialized!");
    return;
  }

  // Get the node containing the components
  auto* node = getNode(a_nodeHandle);
  if (!node)
  {
    FLURR_LOG_ERROR("Unable to destroy components; node %u does not exist!", a_nodeHandle);
    return;
  }

  auto componentHandles = node->getAllComponentHandles();
  for (const FlurrHandle componentHandle : componentHandles)
    destroyComponent(componentHandle);
}

bool SceneManager::hasComponent(FlurrHandle a_componentHandle) const
{
  return m_components.find(a_componentHandle) != m_components.end();
}

NodeComponent* SceneManager::getComponent(FlurrHandle a_componentHandle) const
{
  const auto componentIt = m_components.find(a_componentHandle);
  return componentIt != m_components.end() ? componentIt->second.get() : nullptr;
}

std::vector<FlurrHandle> SceneManager::getAllComponentHandles() const
{
  std::vector<FlurrHandle> componentHandles;
  componentHandles.reserve(m_components.size());
  for (const auto& componentKvp : m_components)
    componentHandles.push_back(componentKvp.first);

  return componentHandles;
}

std::vector<FlurrHandle> SceneManager::getAllComponentHandlesOfType(NodeComponentType a_componentType) const
{
  std::vector<FlurrHandle> componentHandles;
  componentHandles.reserve(m_components.size());
  auto componentHandleRange = m_componentHandlesByType.equal_range(a_componentType);
  for (auto componentHandleIt = componentHandleRange.first; componentHandleIt != componentHandleRange.second; ++componentHandleIt)
    componentHandles.push_back(componentHandleIt->second);

  return componentHandles;
}

CameraComponent* SceneManager::getActiveCamera() const
{
  return m_activeCameraHandle != INVALID_HANDLE ? static_cast<CameraComponent*>(getComponent(m_activeCameraHandle)) : nullptr;
}

void SceneManager::setActiveCameraHandle(FlurrHandle cameraHandle)
{
  if (cameraHandle == INVALID_HANDLE)
  {
    m_activeCameraHandle = cameraHandle;
    FLURR_LOG_INFO("No active camera set.");
    return;
  }

  auto* component = getComponent(cameraHandle);
  FLURR_ASSERT(component, "Component %u does not exist!", cameraHandle);
  FLURR_ASSERT(component->getComponentType() == NodeComponentType::kCamera, "Component %u must be a camera!", cameraHandle);  
  m_activeCameraHandle = cameraHandle;
  FLURR_LOG_INFO("Active camera set to %u.", cameraHandle);
}

Status SceneManager::createNodeWithHandle(FlurrHandle a_nodeHandle, const std::string& a_nodeName, FlurrHandle a_parentNodeHandle,
  const glm::vec3& a_position, const glm::quat& a_rotation, const glm::vec3& a_scale)
{
  // Create node
  auto* node = new Node(a_nodeHandle, a_nodeName, a_parentNodeHandle, a_position, a_rotation, a_scale, this);
  m_nodes[a_nodeHandle] = std::unique_ptr<Node>(node);
  m_nodeHandlesByName[a_nodeName] = a_nodeHandle;

  // Parent node
  auto* parentNode = INVALID_HANDLE != a_parentNodeHandle ? getNode(a_parentNodeHandle) : nullptr;
  if (parentNode)
  {
    parentNode->addChildNode(a_nodeHandle);
    FLURR_LOG_INFO("Created node %s (%u) with parent %s (%u).", a_nodeName.c_str(), a_nodeHandle,
      parentNode->getNodeName().c_str(), a_parentNodeHandle);
  }
  else
  {
    FLURR_ASSERT(a_nodeHandle == ROOT_NODE_HANDLE && a_nodeName == ROOT_NODE_NAME,
      "Only root node can be parent-less!");
    FLURR_LOG_INFO("Created root node %s (%u).", a_nodeName.c_str(), a_nodeHandle);
  }

  // Initialize node
  Status result = node->initNode();
  if (Status::kSuccess != result)
  {
    FLURR_LOG_ERROR("Failed to initialize node %s (%u) !", a_nodeName.c_str(), a_nodeHandle);
    return result;
  }

  return Status::kSuccess;
}

void SceneManager::destroyEmptyNode(FlurrHandle a_nodeHandle)
{
  // Deinitialize the node
  auto* node = getNode(a_nodeHandle);
  node->destroyNode();

  // Unparent the specified node
  const FlurrHandle parentNodeHandle = node->getParentNodeHandle();
  auto* parentNode = getNode(parentNodeHandle);
  if (parentNode)
  {
    parentNode->m_childNodeHandles.erase(
      std::remove(parentNode->m_childNodeHandles.begin(), parentNode->m_childNodeHandles.end(), a_nodeHandle),
      parentNode->m_childNodeHandles.end()
    );
  }

  // Delete the specified node
  const std::string nodeName = node->getNodeName();
  m_nodeHandlesByName.erase(nodeName);
  m_nodes.erase(a_nodeHandle);
}

void SceneManager::removeComponent(FlurrHandle a_componentHandle)
{
  // Remove component from containing node
  auto* component = getComponent(a_componentHandle);
  component->getContainingNode()->removeComponent(a_componentHandle);

  // Delete the component
  for (auto componentHandleIt = m_componentHandlesByType.begin(); componentHandleIt != m_componentHandlesByType.end(); ++componentHandleIt)
  {
    if (componentHandleIt->second == a_componentHandle)
    {
      m_componentHandlesByType.erase(componentHandleIt);
      break;
    }
  }
  m_components.erase(a_componentHandle);
}

std::string SceneManager::generateNodeName()
{
  while (m_nextNodeNameIndex + 1)
  {
    std::string nodeName = std::string(NODE_NAME_PREFIX) + std::to_string(m_nextNodeNameIndex++);
    if (!hasNode(nodeName))
      return nodeName;
  }

  return "";
}

NodeComponent* SceneManager::createComponentOfType(FlurrHandle a_componentHandle, FlurrHandle a_nodeHandle, NodeComponentType a_componentType)
{
  switch (a_componentType)
  {
    case NodeComponentType::kCamera:
    {
      return new CameraComponent(a_componentHandle, a_nodeHandle, this);
    }
    default:
    {
      FLURR_ASSERT(false, "Unhandled node component type %u!", FromEnum(a_componentType));
    }
  }
}

} // namespace flurr
