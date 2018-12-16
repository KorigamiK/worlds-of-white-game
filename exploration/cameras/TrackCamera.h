#ifndef WILT_TRACKCAMERA_H
#define WILT_TRACKCAMERA_H

#include "ICamera.h"
#include "../entities/Entity.h"

class TrackCamera : public ICamera
{
public:
  Entity** entity_;

public:
  TrackCamera(Entity** entity);

public:
  // ICamera overrides
  void update(GameState& state, float time) override;
  glm::mat4 getTransform() const override;
  glm::vec3 getPosition() const override;
  float getAngle() const override;
};

#endif // !WILT_TRACKCAMERA_H
