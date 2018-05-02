#ifndef WILT_JOINT_H
#define WILT_JOINT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Joint
{
public:
  int _parentIndex;
  glm::mat4 _transform;

public:
  Joint();
  Joint(int parentIndex, glm::vec3 location, glm::quat rotation);
  Joint(const Joint&) = default;
  Joint(Joint&&) = default;
  Joint& operator= (const Joint&) = default;
  Joint& operator= (Joint&&) = default;

public:
  int parentIndex() const;
  const glm::mat4& transform() const;

}; // class Joint

#endif // !WILT_JOINT_H
