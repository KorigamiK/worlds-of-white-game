#include "TrackCamera.h"


TrackCamera::TrackCamera(ModelInstance** instance)
  : _instance{ instance }
{ }

void TrackCamera::update(GLFWwindow *window, float time, int selectedJoystickId)
{

}

glm::mat4 TrackCamera::transform() const
{
  return glm::lookAt({ 0, 0, 1 }, (*_instance)->position, { 0, 0, 1 });
}

glm::vec3 TrackCamera::position() const
{
  return { 0, 0, 1 };
}
