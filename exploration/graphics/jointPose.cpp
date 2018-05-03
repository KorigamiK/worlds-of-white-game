#include "jointPose.h"

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

JointPose::JointPose(glm::vec3 location, glm::quat rotation)
  : location{ location }
  , rotation{ rotation }
{ }

glm::mat4 JointPose::calcLocalTransform() const
{
  glm::mat4 locationTransform = glm::translate(glm::mat4(), { location.x, -location.z, location.y });
  glm::mat4 rotationTransform = (glm::mat4)glm::quat(rotation.w, rotation.x, -rotation.z, rotation.y);

  return locationTransform * rotationTransform;
}

JointPose JointPose::interpolate(const JointPose& joint1, const JointPose& joint2, float interp)
{
  glm::vec3 location = glm::mix(joint1.location, joint2.location, interp);
  glm::quat rotation = glm::mix(joint1.rotation, joint2.rotation, interp);

  glm::mat4 locationTransform = glm::translate(glm::mat4(), { location.x, -location.z, location.y });
  glm::mat4 rotationTransform = (glm::mat4)glm::quat(rotation.w, rotation.x, -rotation.z, rotation.y);

  return JointPose{ location, rotation };
}
