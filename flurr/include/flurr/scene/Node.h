#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/scene/NodeComponent.h"
#include "flurr/scene/SceneManager.h"
#include "flurr/utils/MathUtils.h"

#include <glm/gtc/quaternion.hpp>

#include <unordered_set>
#include <vector>

namespace flurr
{

class SceneManager;

class FLURR_DLL_EXPORT Node
{

  friend class SceneManager;

protected:

  Node(FlurrHandle a_nodeHandle, const std::string& a_nodeName, FlurrHandle a_parentNodeHandle,
    const glm::vec3& a_position, const glm::quat& a_rotation, const glm::vec3& a_scale, SceneManager* a_owningManager);
  Node(const Node&) = delete;
  Node(Node&&) = delete;
  Node& operator=(const Node&) = delete;
  Node& operator=(Node&&) = delete;

public:

  ~Node() = default; // needed so unique_ptr can delete Node objects

  FlurrHandle getNodeHandle() const { return m_nodeHandle; }
  const std::string& getNodeName() const { return m_nodeName; }
  SceneManager* getOwningManager() const { return m_owningManager; }
  FlurrHandle getParentNodeHandle() const { return m_nodeHandle; }
  Node* getParentNode() const { return m_owningManager->getNode(m_parentNodeHandle); }
  Status addChildNode(FlurrHandle a_nodeHandle);
  void removeChildNode(FlurrHandle a_nodeHandle);
  bool hasChildNode(FlurrHandle a_nodeHandle) const;
  Node* getChildNode(std::size_t a_childNodeIndex) const;
  std::size_t getChildNodeCount() const { return m_childNodeHandles.size(); }
  std::vector<FlurrHandle> getAllChildNodeHandles() const { return m_childNodeHandles; }
  bool isRootNode() const { return m_nodeHandle == ROOT_NODE_HANDLE; }

  bool hasComponent(FlurrHandle a_componentHandle) const;
  NodeComponent* getComponent(std::size_t a_componentIndex) const;
  std::size_t getComponentCount() const { return m_componentHandles.size(); }
  std::vector<FlurrHandle> getAllComponentHandles() const { return m_componentHandles; }

  const glm::vec3& getPosition() const { return m_position; }
  void setPosition(const glm::vec3& a_position, bool a_updateWorldTransform = true);
  const glm::quat& getRotation() const { return m_rotation; }
  void setRotation(const glm::quat& a_rotation, bool a_updateWorldTransform = true);
  const glm::vec3& getScale() const { return m_scale; }
  void setScale(const glm::vec3& a_scale, bool a_updateWorldTransform = true);
  const glm::vec3& getWorldPosition() const { return m_worldPosition; }
  const glm::quat& getWorldRotation() const { return m_worldRotation; }
  const glm::quat& getWorldScale() const { return m_worldScale; }
  void updateWorldTransform();
  void translate(const glm::vec3& a_translation, bool a_updateWorldTransform = true);
  void rotate(const glm::quat& a_rotation, bool a_updateWorldTransform = true);
  void scale(const glm::vec3& a_scale, bool a_updateWorldTransform = true);
  void lookAt(const glm::vec3& a_targetWorldPosition, const glm::vec3& a_worldUp = MathUtils::UP);

private:

  Status initNode();
  void destroyNode();
  Status updateNode(float a_deltaTime);
  Status drawNode();
  void setParentNode(FlurrHandle a_parentNodeHandle);
  void addComponent(FlurrHandle a_componentHandle);
  void removeComponent(FlurrHandle a_componentHandle);
  void removeAllComponents();

  FlurrHandle m_nodeHandle;
  std::string m_nodeName;
  SceneManager* m_owningManager;
  FlurrHandle m_parentNodeHandle;
  std::vector<FlurrHandle> m_childNodeHandles;
  std::vector<FlurrHandle> m_componentHandles;

  glm::vec3 m_position;
  glm::quat m_rotation;
  glm::vec3 m_scale;
  glm::vec3 m_worldPosition;
  glm::quat m_worldRotation;
  glm::vec3 m_worldScale;
};

} // namespace flurr
