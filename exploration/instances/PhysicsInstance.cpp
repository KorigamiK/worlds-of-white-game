#include "PhysicsInstance.h"

PhysicsInstance::PhysicsInstance(Model* model, const InstanceSpawnInfo& info, btRigidBody* body)
  : Instance{ model, info }
  , body{ body }
{ }

btRigidBody* PhysicsInstance::getBody()
{
  return body;
}

void PhysicsInstance::update(GameState& state, float time)
{
  btTransform bodyTransform;
  body->getMotionState()->getWorldTransform(bodyTransform);
  btVector3 bodyPosition = bodyTransform.getOrigin();
  position = glm::vec3(bodyPosition.x(), bodyPosition.y(), bodyPosition.z());
}
