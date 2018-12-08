#ifndef WILT_FOLLOWCAMERA_H
#define WILT_FOLLOWCAMERA_H

#include "ICamera.h"
#include "../graphics/modelInstance.h"

class FollowCamera : public ICamera
{
public:
  ModelInstance** _instance;
  float optimalAngle;
  float optimalDistance;
  glm::vec3 optimalPosition; // world space
  glm::vec3 currentPosition; // world space

public:
  FollowCamera(ModelInstance** instance);

public:
  // ICamera overrides
  void update(GLFWwindow *window, float time, int selectedJoystickId) override;
  glm::mat4 getTransform() const override;
  glm::vec3 getPosition() const override;
  float getAngle() const override;
};

#endif // !WILT_FOLLOWCAMERA_H
