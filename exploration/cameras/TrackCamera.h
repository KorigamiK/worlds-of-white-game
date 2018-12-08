#ifndef WILT_TRACKCAMERA_H
#define WILT_TRACKCAMERA_H

#include "ICamera.h"
#include "../graphics/modelInstance.h"

class TrackCamera : public ICamera
{
public:
  ModelInstance** _instance;

public:
  TrackCamera(ModelInstance** instance);

  // ICamera overrides
public:
  void update(GLFWwindow *window, float time, int selectedJoystickId) override;
  glm::mat4 transform() const override;
  glm::vec3 position() const override;
};

#endif // !WILT_TRACKCAMERA_H
