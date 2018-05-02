#include "joint.h"

Joint::Joint()
  : _parentIndex{ -1 }
{ }

Joint::Joint(int parentIndex, glm::vec3 location, glm::quat rotation)
  : _parentIndex{ parentIndex }
{
  glm::mat4 rotationTransform = (glm::mat4)glm::quat(rotation.w, rotation.x, -rotation.z, rotation.y);
  glm::mat4 locationTransform = glm::translate(glm::mat4(), { location.x, -location.z, location.y });

  _transform = locationTransform * rotationTransform;
}

int Joint::parentIndex() const
{
  return _parentIndex;
}

const glm::mat4& Joint::transform() const
{
  return _transform;
}
