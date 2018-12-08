#ifndef WILT_FOLLOWCAMERA_H
#define WILT_FOLLOWCAMERA_H

#include "ICamera.h"
#include "../graphics/modelInstance.h"

class FollowCamera : public ICamera
{
public:
  ModelInstance** _instance;
  float distance;
  float offsetAngle;

public:
  FollowCamera(ModelInstance** instance);

  // ICamera overrides
public:
  void update(GLFWwindow *window, float time, int selectedJoystickId) override;
  glm::mat4 transform() const override;
  glm::vec3 position() const override;
};

#endif // !WILT_FOLLOWCAMERA_H
