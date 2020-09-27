#include "flurr/scene/CameraComponent.h"
#include "flurr/scene/Node.h"
#include "flurr/FlurrCore.h"
#include "flurr/FlurrLog.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace flurr
{

CameraComponent::CameraComponent(FlurrHandle a_componentHandle, FlurrHandle a_containingNodeHandle, SceneManager* a_owningManager)
  : NodeComponent(a_componentHandle, a_containingNodeHandle, a_owningManager)
{
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::getViewport(int& a_x, int& a_y, uint32_t& a_w, uint32_t& a_h) const
{
  a_x = m_vpx;
  a_y = m_vpy;
  a_w = m_vpw;
  a_h = m_vph;
}

void CameraComponent::setViewport(int a_x, int a_y, uint32_t a_w, uint32_t a_h)
{
  m_vpx = a_x;
  m_vpy = a_y;
  m_vpw = glm::clamp<uint32_t>(a_w, 1, kMaxViewportSize);
  m_vph = glm::clamp<uint32_t>(a_h, 1, kMaxViewportSize);
}

Status CameraComponent::onInitComponent(const NodeComponentInitArgs& a_initArgs)
{
  const auto& cameraInitArgs = static_cast<const CameraComponentInitArgs&>(a_initArgs);
  setCameraType(cameraInitArgs.cameraType);
  setFieldOfView(cameraInitArgs.fov);
  setViewport(cameraInitArgs.vpx, cameraInitArgs.vpy, cameraInitArgs.vpw, cameraInitArgs.vph);
  setNearClipDistance(cameraInitArgs.ncd);
  setFarClipDistance(cameraInitArgs.fcd);

  return Status::kSuccess;
}

void CameraComponent::onDestroyComponent()
{
}

Status CameraComponent::onUpdateComponent(float a_deltaTime)
{
  return Status::kSuccess;
}

Status CameraComponent::onDrawComponent()
{
  // Configure viewport
  auto* renderer = FlurrCore::Get().getRenderer();
  renderer->setViewport(m_vpx, m_vpy, m_vpw, m_vph);

  // Compute view-projection transforms
  auto* cameraNode = getContainingNode();
  glm::mat4 viewTransf = glm::toMat4(glm::conjugate(cameraNode->getWorldRotation()));
  viewTransf = glm::translate(viewTransf, -cameraNode->getWorldPosition());
  const glm::mat4 projTransf = getCameraType() == CameraType::kPerspective ?
    glm::perspective(getFieldOfView(), getAspectRatio(), getNearClipDistance(), getFarClipDistance()) :
    glm::ortho(m_vpx, m_vpx + m_vpw, m_vpy, m_vpy + m_vph);
  const glm::mat4 viewProjTransf = projTransf * viewTransf;

  // Set view-projection transform
  for (std::size_t programIndex = 0; programIndex < renderer->getShaderProgramCount(); ++programIndex)
  {
    auto* program = renderer->getShaderProgramByIndex(programIndex);
    program->useProgram();
    if (!program->setMat4Value(VIEW_PROJECTION_TRANSFORM_UNIFORM_NAME, viewProjTransf))
    {
      FLURR_LOG_WARN("Unable to set view-projection transform on shader program %u; failed to resolve uniform %s!",
        program->getProgramHandle(), VIEW_PROJECTION_TRANSFORM_UNIFORM_NAME);
    }
  }

  return Status::kSuccess;
}

} // namespace flurr
