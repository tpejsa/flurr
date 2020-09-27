#include "flurr/scene/Node.h"
#include "flurr/scene/SceneManager.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/MathUtils.h"

#include <algorithm>

namespace flurr
{

Node::Node(FlurrHandle a_nodeHandle, const std::string& a_nodeName, FlurrHandle a_parentNodeHandle,
  const glm::vec3& a_position, const glm::quat& a_rotation, const glm::vec3& a_scale, SceneManager* a_owningManager)
  : m_nodeHandle(a_nodeHandle),
  m_nodeName(a_nodeName),
  m_parentNodeHandle(a_parentNodeHandle),
  m_position(a_position),
  m_rotation(a_rotation),
  m_scale(a_scale)
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

  m_childNodeHandles.push_back(a_nodeHandle);
  childNode->setParentNode(getNodeHandle());
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

  m_childNodeHandles.erase(std::remove(m_childNodeHandles.begin(), m_childNodeHandles.end(), a_nodeHandle), m_childNodeHandles.end());
  childNode->setParentNode(ROOT_NODE_HANDLE);
}

bool Node::hasChildNode(FlurrHandle a_nodeHandle) const
{
  const auto childIt = std::find(m_childNodeHandles.begin(), m_childNodeHandles.end(), a_nodeHandle);
  return childIt != m_childNodeHandles.end();
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

void Node::setPosition(const glm::vec3& a_position, bool a_updateWorldTransform)
{
  m_position = a_position;
  if (a_updateWorldTransform)
    updateWorldTransform();
}

void Node::setRotation(const glm::quat& a_rotation, bool a_updateWorldTransform)
{
  m_rotation = a_rotation;
  if (a_updateWorldTransform)
    updateWorldTransform();
}

void Node::setScale(const glm::vec3& a_scale, bool a_updateWorldTransform)
{
  m_scale = a_scale;
  if (a_updateWorldTransform)
    updateWorldTransform();
}

void Node::updateWorldTransform()
{
  auto* parentNode = getParentNode();
  m_worldRotation = parentNode ? (parentNode->getWorldRotation() * m_rotation) : m_rotation;
  m_worldPosition = parentNode ? (parentNode->getWorldPosition() + parentNode->getWorldRotation() * m_position) : m_position;
  m_worldScale = parentNode ? (parentNode->getWorldScale() * m_scale) : m_scale;

  // Update world transforms of children also
  for (const FlurrHandle childNodeHandle : m_childNodeHandles)
  {
    auto* childNode = m_owningManager->getNode(childNodeHandle);
    childNode->updateWorldTransform();
  }
}

void Node::translate(const glm::vec3& a_translation, bool a_updateWorldTransform)
{
  m_position += a_translation;
  if (a_updateWorldTransform)
    updateWorldTransform();
}

void Node::rotate(const glm::quat& a_rotation, bool a_updateWorldTransform)
{
  m_rotation = a_rotation * m_rotation;
  if (a_updateWorldTransform)
    updateWorldTransform();
}

void Node::scale(const glm::vec3& a_scale, bool a_updateWorldTransform)
{
  m_scale *= a_scale;
  if (a_updateWorldTransform)
    updateWorldTransform();
}

void Node::lookAt(const glm::vec3& a_targetWorldPosition, const glm::vec3& a_worldUp)
{
	// Compute target forward
	const glm::vec3 targetWorldForward = glm::normalize(a_targetWorldPosition - m_worldPosition);

	// Compute rotation to target
	const glm::quat worldRot = MathUtils::RotationFromTo(MathUtils::FORWARD, targetWorldForward);

	// Compute the up vector rotation
	const glm::vec3 rotatedWorldUp = worldRot * MathUtils::UP;
	glm::quat upWorldRot(MathUtils::IDENTITY);
	glm::vec3 upTargetNormal = glm::cross(a_worldUp, targetWorldForward);
	if (glm::length2(upTargetNormal) > glm::epsilon<float>())
	{
		const glm::vec3 projWorldUp = glm::normalize(MathUtils::ProjectVectorToPlane(rotatedWorldUp, upTargetNormal));
		upWorldRot = MathUtils::RotationFromTo(rotatedWorldUp, projWorldUp);
	}

	// Compute final rotation
	m_worldRotation = upWorldRot * worldRot;
	m_rotation = isRootNode() ? m_worldRotation : (glm::conjugate(getParentNode()->getWorldRotation()) * m_worldRotation);

	// Update child world transforms
	for (const FlurrHandle childNodeHandle : m_childNodeHandles)
		m_owningManager->getNode(childNodeHandle)->updateWorldTransform();
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

void Node::setParentNode(FlurrHandle a_parentNodeHandle)
{
  m_parentNodeHandle = a_parentNodeHandle;
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
