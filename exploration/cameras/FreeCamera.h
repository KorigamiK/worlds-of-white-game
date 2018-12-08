#ifndef WILT_FREECAMERA_H
#define WILT_FREECAMERA_H

#include "ICamera.h"

class FreeCamera : public ICamera
{
private:
  glm::vec3 _location;
  glm::vec3 _direction;

  float CAMERA_SPEED;

public:
  FreeCamera();

  // ICamera overrides
public:
  void update(GLFWwindow *window, float time, int selectedJoystickId) override;
  glm::mat4 transform() const override;
  glm::vec3 position() const override;
};

#endif // !WILT_FREECAMERA_H
