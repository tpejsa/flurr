#pragma once

#include "flurr/FlurrDefines.h"
#include "flurr/scene/NodeComponent.h"

#include <glm/glm.hpp>

namespace flurr
{

constexpr float DEFAULT_CAMERA_FIELD_OF_VIEW = glm::radians(45.0f);
constexpr uint32_t DEFAULT_CAMERA_VIEWPORT_WIDTH = 1920;
constexpr uint32_t DEFAULT_CAMERA_VIEWPORT_HEIGHT = 1080;
constexpr float DEFAULT_CAMERA_NEAR_CLIP_DISTANCE = 0.03f;
constexpr float DEFAULT_CAMERA_FAR_CLIP_DISTANCE = 100.0f;

enum class CameraType : uint8_t
{
  kPerspective = 0,
  kOrthographic
};

struct CameraComponentInitArgs : public NodeComponentInitArgs
{
  NodeComponentType componentType() const override { return NodeComponentType::kCamera; }
  CameraType cameraType = CameraType::kPerspective;
  float fov = DEFAULT_CAMERA_FIELD_OF_VIEW;
  int vpx = 0;
  int vpy = 0;
  uint32_t vpw = DEFAULT_CAMERA_VIEWPORT_WIDTH;
  uint32_t vph = DEFAULT_CAMERA_VIEWPORT_HEIGHT;
  float ncd = DEFAULT_CAMERA_NEAR_CLIP_DISTANCE;
  float fcd = DEFAULT_CAMERA_FAR_CLIP_DISTANCE;
};

class FLURR_DLL_EXPORT CameraComponent : public NodeComponent
{

  friend class SceneManager;

protected:

  CameraComponent(FlurrHandle a_componentHandle, FlurrHandle a_containingNodeHandle, SceneManager* a_owningManager);

public:

  ~CameraComponent() override; // needed so unique_ptr can delete NodeComponent objects

  NodeComponentType getComponentType() const override { return NodeComponentType::kCamera; }
  CameraType getCameraType() const { return m_cameraType; }
  void setCameraType(CameraType a_cameraType) { m_cameraType = a_cameraType; }
  float getFieldOfView() const { return m_fov; }
  void setFieldOfView(float a_fov) { m_fov = glm::clamp(a_fov, 0.0f, glm::two_pi<float>()); m_projTransfDirty = true; }
  void getViewport(int& a_x, int& a_y, uint32_t& a_w, uint32_t& a_h) const;
  void setViewport(int a_x, int a_y, uint32_t a_w, uint32_t a_h);
  float getNearClipDistance() const { return m_ncd; }
  void setNearClipDistance(float a_ncd) { m_ncd = glm::clamp(a_ncd, kMinClipDistance, kMaxClipDistance); m_projTransfDirty = true; }
  float getFarClipDistance() const { return m_fcd; }
  void setFarClipDistance(float a_fcd) { m_fcd = glm::clamp(a_fcd, kMinClipDistance, kMaxClipDistance); m_projTransfDirty = true; }
  float getAspectRatio() const { return ((float) m_vpw) / m_vph; }
  void applyRendererViewport();
  void applyShaderViewProjectionMatrix(FlurrHandle a_shaderProgramHandle);

private:

  Status onInitComponent(const NodeComponentInitArgs& a_initArgs) override;
  void onDestroyComponent() override;
  Status onUpdateComponent(float a_deltaTime) override;

  static constexpr uint32_t kMaxViewportSize = 100000;
  static constexpr float kMinClipDistance = 0.00001f;
  static constexpr float kMaxClipDistance = 100000.0f;

  CameraType m_cameraType;
  float m_fov; // in radians
  int m_vpx, m_vpy, m_vpw, m_vph; // viewport
  float m_ncd, m_fcd; // near and far clipping distances
  glm::mat4 m_projTransf;
  bool m_projTransfDirty;
};

} // namespace flurr
