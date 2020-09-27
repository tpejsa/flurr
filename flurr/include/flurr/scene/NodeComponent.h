#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/scene/SceneManager.h"

namespace flurr
{

class Node;

class FLURR_DLL_EXPORT NodeComponent
{

  friend class SceneManager;
  friend class Node;

protected:

  NodeComponent(FlurrHandle a_componentHandle, FlurrHandle a_containingNodeHandle, SceneManager* a_owningManager);
  NodeComponent(const NodeComponent&) = delete;
  NodeComponent(NodeComponent&&) = delete;
  NodeComponent& operator=(const NodeComponent&) = delete;
  NodeComponent& operator=(NodeComponent&&) = delete;

public:

  virtual ~NodeComponent(); // needed so unique_ptr can delete NodeComponent objects

  virtual NodeComponentType getComponentType() const = 0;
  virtual Priority getComponentPriority() const { return NORMAL_PRIORITY; }
  FlurrHandle getComponentHandle() const { return m_componentHandle; }
  FlurrHandle getContainingNodeHandle() const { return m_containingNodeHandle; }
  Node* getContainingNode() const { return m_owningManager->getNode(m_containingNodeHandle); }
  SceneManager* getOwningManager() const { return m_owningManager; }
  bool getEnabled() const { return m_enabled; }
  void setEnabled(bool a_enabled) { m_enabled = a_enabled; }

private:

  Status initComponent(const NodeComponentInitArgs& a_initArgs);
  void destroyComponent();
  Status updateComponent(float a_deltaTime);
  Status drawComponent();

  virtual Status onInitComponent(const NodeComponentInitArgs& a_initArgs) = 0;
  virtual void onDestroyComponent() = 0;
  virtual Status onUpdateComponent(float a_deltaTime) = 0;
  virtual Status onDrawComponent() { return Status::kSuccess; }

  FlurrHandle m_componentHandle;
  FlurrHandle m_containingNodeHandle;
  SceneManager* m_owningManager;
  bool m_enabled;
};

} // namespace flurr
