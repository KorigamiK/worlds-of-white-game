#ifndef WILT_JOINTPOSE_H
#define WILT_JOINTPOSE_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class JointPose
{
public:
  glm::vec3 location;
  glm::quat rotation;

public:
  JointPose() = default;
  JointPose(glm::vec3 location, glm::quat rotation);
  JointPose(const JointPose&) = default;
  JointPose(JointPose&&) = default;
  JointPose& operator= (const JointPose&) = default;
  JointPose& operator= (JointPose&&) = default;

public:
  glm::mat4 calcLocalTransform() const;

public:
  static JointPose interpolate(const JointPose& joint1, const JointPose& joint2, float interp);

}; // class JointPose

#endif // !WILT_JOINTPOSE_H
