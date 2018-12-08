#include "TrackCamera.h"


TrackCamera::TrackCamera(ModelInstance** instance)
  : _instance{ instance }
{ }

void TrackCamera::update(GLFWwindow *window, float time, int selectedJoystickId)
{

}

glm::mat4 TrackCamera::getTransform() const
{
  return glm::lookAt({ 0, 0, 1 }, (*_instance)->position, { 0, 0, 1 });
}

glm::vec3 TrackCamera::getPosition() const
{
  return { 0, 0, 1 };
}

float TrackCamera::getAngle() const
{
  return 0.0f;
}
