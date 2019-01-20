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

public:
  // ICamera overrides
  void update(GameState& state, float time) override;
  glm::mat4 getTransform() const override;
  glm::vec3 getPosition() const override;
  glm::vec3 getDirection() const override;
  float getAngle() const override;
};

#endif // !WILT_FREECAMERA_H
