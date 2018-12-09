#ifndef WILT_ICAMERA_H
#define WILT_ICAMERA_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "../GameState.h"

class ICamera
{
public:
  virtual void update(GameState& state, float time) = 0;
  virtual glm::mat4 getTransform() const = 0;
  virtual glm::vec3 getPosition() const = 0;
  virtual float getAngle() const = 0;
};

#endif // !WILT_ICAMERA_H
