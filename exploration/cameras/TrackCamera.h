#ifndef WILT_TRACKCAMERA_H
#define WILT_TRACKCAMERA_H

#include "ICamera.h"
#include "../graphics/modelInstance.h"

class TrackCamera : public ICamera
{
public:
  ModelInstance** _instance;

public:
  TrackCamera(ModelInstance** instance)
    : _instance{ instance }
  { }

public:
  void update(GLFWwindow *window, float time) override
  {

  }

  glm::mat4 transform() const override
  {
    return glm::lookAt({ 0, 0, 1 }, (*_instance)->position, { 0, 0, 1 });
  }

  glm::vec3 position() const override
  {
    return { 0, 0, 1 };
  }
};

#endif // !WILT_TRACKCAMERA_H
