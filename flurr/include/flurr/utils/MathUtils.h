#pragma once

#include "flurr/FlurrDefines.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

#if (GLM_ARCH == GLM_ARCH_PURE)
#define FLURR_GLM_CONSTEXPR constexpr
#else
#define FLURR_GLM_CONSTEXPR const
#endif

namespace flurr
{
namespace MathUtils
{

constexpr float EPS = 0.00001f;
FLURR_GLM_CONSTEXPR glm::vec3 X_AXIS{1.0f, 0.0f, 0.0f};
FLURR_GLM_CONSTEXPR glm::vec3 Y_AXIS{0.0f, 1.0f, 0.0f};
FLURR_GLM_CONSTEXPR glm::vec3 Z_AXIS{0.0f, 0.0f, 1.0f};
FLURR_GLM_CONSTEXPR glm::vec3 RIGHT{X_AXIS};
FLURR_GLM_CONSTEXPR glm::vec3 UP{Y_AXIS};
FLURR_GLM_CONSTEXPR glm::vec3 FORWARD{-Z_AXIS};
FLURR_GLM_CONSTEXPR glm::vec3 ZERO{0.0f, 0.0f, 0.0f};
FLURR_GLM_CONSTEXPR glm::quat IDENTITY{1.0f, 0.0f, 0.0f, 0.0f};

inline glm::mat4 TRS(const glm::vec3& trans, const glm::quat& rot, const glm::vec3& scale)
{
  return glm::translate(glm::toMat4(rot) * glm::scale(scale), trans);
}

inline glm::vec3 FindOrthogonalVector(const glm::vec3& v)
{
  int m = 0;
  while (m < 3 && glm::abs(v[m]) <= glm::epsilon<float>())
    ++m;
  if (m >= 3)
    return ZERO;
  int n = (m + 1) % 3;

  glm::vec3 vn;
  vn[n] = v[m];
  vn[m] = -v[n];
  return glm::normalize(vn);
}

inline glm::quat RotationFromTo(const glm::vec3& v1, const glm::vec3& v2)
{
  glm::vec3 axis = glm::cross(v1, v2);
  if (glm::length2(axis) <= glm::epsilon<float>())
    axis = FindOrthogonalVector(v1);

  const glm::quat rot(glm::sqrt(glm::length2(v1)*glm::length2(v2)) + glm::dot(v1, v2), axis);
  return glm::normalize(rot);
}

inline glm::vec3 ProjectVector(const glm::vec3& v, const glm::vec3& vt)
{
  return glm::dot(v, vt) / glm::length2(vt) * v;
}

inline glm::vec3 ProjectVectorToPlane(const glm::vec3& v, const glm::vec3& vn)
{
  const glm::vec3 vproj = ProjectVector(v, vn);
  return v - vproj;
}

inline float MapAngleTwoPi(float angle)
{
  if (angle >= 0.0f && angle <= glm::two_pi<float>())
  {
    return angle;
  }
  else if (angle > glm::two_pi<float>())
  {
    return angle - floor(angle / glm::two_pi<float>()) * glm::two_pi<float>();
  }
  else
  {
    return glm::two_pi<float>() - MapAngleTwoPi(-angle);
  }
}

inline float MapAnglePi(float angle)
{
  if (angle >= -glm::pi<float>() && angle <= glm::pi<float>())
  {
    return angle;
  }
  else if (angle > glm::pi<float>())
  {
    return MapAngleTwoPi(angle + glm::pi<float>()) - glm::pi<float>();
  }
  else
  {
    return -MapAnglePi(-angle);
  }
}

} // namespace MathUtils
} // namespace flurr
