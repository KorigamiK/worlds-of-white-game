#ifndef WILT_IDLECAMERA_H
#define WILT_IDLECAMERA_H

#include <chrono>

#include "ICamera.h"

class Entity;

class IdleCamera : public ICamera
{
private:
  Entity** entity_;
  float desiredXAngle_;
  float desiredYAngle_;
  float desiredDistance_;
  glm::vec3 desiredPosition_; // world space
  glm::vec3 currentPosition_; // world space
  std::chrono::time_point<std::chrono::high_resolution_clock> lastInputTime;

public:
  IdleCamera(Entity** entity);

public:
  // ICamera overrides
  void update(GameState& state, float time) override;
  glm::mat4 getTransform() const override;
  glm::vec3 getPosition() const override;
  glm::vec3 getDirection() const override;
  float getAngle() const override;
};

#endif // !WILT_IDLECAMERA_H
