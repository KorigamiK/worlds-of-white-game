#include "TrackCamera.h"


TrackCamera::TrackCamera(Entity** entity)
  : entity_{ entity }
{ }

void TrackCamera::update(GameState& state, float time)
{

}

glm::mat4 TrackCamera::getTransform() const
{
  return glm::lookAt({ 0, 0, 1 }, (*entity_)->position, { 0, 0, 1 });
}

glm::vec3 TrackCamera::getPosition() const
{
  return { 0, 0, 1 };
}

glm::vec3 TrackCamera::getDirection() const
{
  return glm::normalize((*entity_)->position - glm::vec3(0, 0, 1));
}

float TrackCamera::getAngle() const
{
  return 0.0f;
}
