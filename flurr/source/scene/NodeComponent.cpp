#include "flurr/scene/NodeComponent.h"
#include "flurr/FlurrLog.h"

namespace flurr
{

NodeComponent::NodeComponent(FlurrHandle a_componentHandle, FlurrHandle a_containingNodeHandle, SceneManager* a_owningManager)
  : m_componentHandle(a_componentHandle),
  m_containingNodeHandle(a_containingNodeHandle),
  m_owningManager(a_owningManager),
  m_enabled(true)
{
}

NodeComponent::~NodeComponent()
{
}

Status NodeComponent::initComponent(const NodeComponentInitArgs& a_initArgs)
{
  FLURR_ASSERT(getComponentType() == a_initArgs.componentType(), "Component type mismatch!");
  return onInitComponent(a_initArgs);
}

void NodeComponent::destroyComponent()
{
  onDestroyComponent();
}

Status NodeComponent::updateComponent(float a_deltaTime)
{
  return getEnabled() ? onUpdateComponent(a_deltaTime) : Status::kSuccess;
}

Status NodeComponent::drawComponent()
{
  return getEnabled() ? onDrawComponent() : Status::kSuccess;
}

} // namespace flurr
