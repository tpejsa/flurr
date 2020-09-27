#include "flurr/scene/Node.h"
#include "flurr/scene/SceneManager.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/MathUtils.h"

#include <glm/gtc/epsilon.hpp>

#include <algorithm>

namespace flurr
{

Node::Node(FlurrHandle a_nodeHandle, const std::string& a_nodeName, FlurrHandle a_parentNodeHandle,
  const glm::vec3& a_position, const glm::quat& a_rotation, const glm::vec3& a_scale, SceneManager* a_owningManager)
  : m_nodeHandle(a_nodeHandle),
  m_nodeName(a_nodeName),
  m_owningManager(a_owningManager),
  m_parentNodeHandle(a_parentNodeHandle),
  m_position(a_position),
  m_rotation(a_rotation),
  m_scale(a_scale),
  m_transfsDirty(true)
{
}

Status Node::addChildNode(FlurrHandle a_nodeHandle)
{
  auto* childNode = m_owningManager->getNode(a_nodeHandle);
  if (!childNode)
  {
    FLURR_LOG_ERROR("Node %u does not exist!", a_nodeHandle);
    return Status::kInvalidHandle;
  }

  if (hasChildNode(a_nodeHandle))
  {
    FLURR_LOG_WARN("Node %s (%u) is already a child of node %s (%u)!",
      childNode->getNodeName().c_str(), a_nodeHandle, getNodeName().c_str(), getNodeHandle());
    return Status::kSuccess;
  }

  auto* oldParentNode = childNode->getParentNode();
  if (oldParentNode != nullptr)
  {
    oldParentNode->m_childNodeHandles.erase(
      std::remove(oldParentNode->m_childNodeHandles.begin(), oldParentNode->m_childNodeHandles.end(), a_nodeHandle),
      oldParentNode->m_childNodeHandles.end()
    );
  }
    
  childNode->m_parentNodeHandle = getNodeHandle();
  m_childNodeHandles.push_back(a_nodeHandle);
}

void Node::removeChildNode(FlurrHandle a_nodeHandle)
{
  auto* childNode = m_owningManager->getNode(a_nodeHandle);
  if (!childNode)
  {
    FLURR_LOG_ERROR("Node %u does not exist!", a_nodeHandle);
    return;
  }

  if (!hasChildNode(a_nodeHandle))
  {
    FLURR_LOG_WARN("Node %s (%u) is not a child of node %s (%u)!",
      childNode->getNodeName().c_str(), a_nodeHandle, getNodeName().c_str(), getNodeHandle());
    return;
  }

  auto* rootNode = m_owningManager->getRootNode();
  rootNode->addChildNode(a_nodeHandle);  
}

bool Node::hasChildNode(FlurrHandle a_nodeHandle) const
{
  auto* childNode = m_owningManager->getNode(a_nodeHandle);
  return childNode != nullptr && childNode->getParentNodeHandle() == getNodeHandle();
}

Node* Node::getChildNode(std::size_t a_childNodeIndex) const
{
  if (a_childNodeIndex >= getChildNodeCount())
    return nullptr;

  return m_owningManager->getNode(m_childNodeHandles[a_childNodeIndex]);
}

bool Node::hasComponent(FlurrHandle a_componentHandle) const
{
  const auto componentIt = std::find(m_componentHandles.begin(), m_componentHandles.end(), a_componentHandle);
  return componentIt != m_componentHandles.end();
}

NodeComponent* Node::getComponent(std::size_t a_componentIndex) const
{
  if (a_componentIndex >= getComponentCount())
    return nullptr;

  return m_owningManager->getComponent(m_componentHandles[a_componentIndex]);
}

void Node::setPosition(const glm::vec3& a_position)
{
  m_position = a_position;
  setTransformsDirty();
}

void Node::setRotation(const glm::quat& a_rotation)
{
  m_rotation = a_rotation;
  setTransformsDirty();
}

void Node::setScale(const glm::vec3& a_scale)
{
  m_scale = a_scale;
  setTransformsDirty();
}

const glm::mat4& Node::getTransform() const
{
  if (m_transfsDirty) updateTransforms();
  return m_transf;
}

const glm::mat4& Node::getInverseTransform() const
{
  if (m_transfsDirty) updateTransforms();
  return m_invTransf;
}

const glm::vec3& Node::getWorldPosition() const
{
  if (m_transfsDirty) updateTransforms();
  return m_worldPosition;
}

const glm::quat& Node::getWorldRotation() const
{
  if (m_transfsDirty) updateTransforms();
  return m_worldRotation;
}

const glm::quat& Node::getWorldScale() const
{
  if (m_transfsDirty) updateTransforms();
  return m_worldScale;
}

const glm::mat4& Node::getWorldTransform() const
{
  if (m_transfsDirty) updateTransforms();
  return m_worldTransf;
}

const glm::mat4& Node::getInverseWorldTransform() const
{
  if (m_transfsDirty) updateTransforms();
  return m_invWorldTransf;
}

void Node::translate(const glm::vec3& a_translation)
{
  m_position += a_translation;
  setTransformsDirty();
}

void Node::rotate(const glm::quat& a_rotation)
{
  m_rotation = a_rotation * m_rotation;
  setTransformsDirty();
}

void Node::scale(const glm::vec3& a_scale)
{
  m_scale *= a_scale;
  setTransformsDirty();
}

void Node::lookAt(const glm::vec3& a_targetWorldPosition, const glm::vec3& a_worldUp)
{
  // Compute look-at target position and up vector in local space
  const auto targetPos = transformPointToLocal(a_targetWorldPosition);
  if (glm::all(glm::epsilonEqual(targetPos, MathUtils::ZERO, MathUtils::EPS)))
  {
    FLURR_LOG_WARN("Look-at target is at the origin of the node!");
    return;
  }
  const auto up = transformDirectionToLocal(a_worldUp);

  // Calculate look-at rotation  
  m_rotation = glm::quat_cast(glm::lookAt(MathUtils::ZERO, targetPos, up));

  setTransformsDirty();
}

glm::vec3 Node::transformPointToLocal(const glm::vec3& a_worldPosition) const
{
  return getInverseWorldTransform() * glm::vec4(a_worldPosition, 1.0f);
}

glm::vec3 Node::transformDirectionToLocal(const glm::vec3& a_worldDirection) const
{
  return glm::rotate(glm::conjugate(getWorldRotation()), a_worldDirection);
}

glm::vec3 Node::transformPointToWorld(const glm::vec3& a_position) const
{
  return getWorldTransform() * glm::vec4(a_position, 1.0f);
}

glm::vec3 Node::transformDirectionToWorld(const glm::vec3& a_direction) const
{
  return glm::rotate(getWorldRotation(), a_direction);
}

void Node::setTransformsDirty() {
  m_transfsDirty = true;
  for (const FlurrHandle childNodeHandle : m_childNodeHandles)
    m_owningManager->getNode(childNodeHandle)->setTransformsDirty();
}

void Node::updateTransforms() const
{
  // Update local transform matrix
  m_transf = MathUtils::TRS(m_position, m_rotation, m_scale);
  m_invTransf = glm::inverse(m_transf);

  // Update world transform
  auto* parentNode = getParentNode();
  m_worldRotation = parentNode ? (parentNode->getWorldRotation() * m_rotation) : m_rotation;
  m_worldPosition = parentNode ? (parentNode->getWorldPosition() + parentNode->getWorldRotation() * m_position) : m_position;
  m_worldScale = parentNode ? (parentNode->getWorldScale() * m_scale) : m_scale;
  m_worldTransf = MathUtils::TRS(m_worldPosition, m_worldRotation, m_worldScale);
  m_invWorldTransf = glm::inverse(m_worldTransf);

  m_transfsDirty = false;
}

Status Node::initNode()
{
  return Status::kSuccess;
}

void Node::destroyNode()
{
  m_owningManager->destroyAllComponentsOfNode(getNodeHandle());
}

Status Node::updateNode(float a_deltaTime)
{
  // Update components
  for (const FlurrHandle componentHandle : m_componentHandles)
  {
    const Status result = m_owningManager->getComponent(componentHandle)->updateComponent(a_deltaTime);
    if (Status::kSuccess != result)
    {
      FLURR_LOG_ERROR("Failed to update component %u of node %s (%u)!", componentHandle,
        getNodeName().c_str(), getNodeHandle());
      return result;
    }
  }

  // Update children
  for (const FlurrHandle childNodeHandle : m_childNodeHandles)
  {
    auto* childNode = m_owningManager->getNode(childNodeHandle);
    const Status result = childNode->updateNode(a_deltaTime);
    if (Status::kSuccess != result)
    {
      FLURR_LOG_ERROR("Failed to update child node %s (%u) of node %s (%u)!",
        childNode->getNodeName().c_str(), childNode->getNodeHandle(),
        getNodeName().c_str(), getNodeHandle());
      return result;
    }
  }

  return Status::kSuccess;
}

Status Node::drawNode()
{
  // Draw components
  for (const FlurrHandle componentHandle : m_componentHandles)
  {
    const Status result = m_owningManager->getComponent(componentHandle)->drawComponent();
    if (Status::kSuccess != result)
    {
      FLURR_LOG_ERROR("Failed to draw component %u of node %s (%u)!", componentHandle,
        getNodeName().c_str(), getNodeHandle());
      return result;
    }
  }

  return Status::kSuccess;
}

void Node::addComponent(FlurrHandle a_componentHandle)
{
  if (hasComponent(a_componentHandle))
  {
    FLURR_LOG_WARN("Unable to add component %u to node %s (%u); already added!", a_componentHandle,
      getNodeName().c_str(), getNodeHandle());
    return;
  }

  m_componentHandles.push_back(a_componentHandle);
  std::sort(m_componentHandles.begin(), m_componentHandles.end(),
    [this](FlurrHandle a_ch1, FlurrHandle a_ch2) {
      auto* c1 = m_owningManager->getComponent(a_ch1);
      auto* c2 = m_owningManager->getComponent(a_ch2);
      return c1->getComponentPriority() < c2->getComponentPriority();
    });
}

void Node::removeComponent(FlurrHandle a_componentHandle)
{
  m_componentHandles.erase(std::remove(m_componentHandles.begin(), m_componentHandles.end(), a_componentHandle), m_componentHandles.end());
}

void Node::removeAllComponents()
{
  m_componentHandles.clear();
}

} // namespace flurr
