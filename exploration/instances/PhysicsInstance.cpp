#include "PhysicsInstance.h"

PhysicsInstance::PhysicsInstance(Model* model, glm::vec3 position, float rotation, btRigidBody* body, float scale)
  : Instance{ model, position, rotation, scale }
  , body{ body }
{ }

void PhysicsInstance::update(GameState& state, float time)
{
  btTransform bodyTransform;
  body->getMotionState()->getWorldTransform(bodyTransform);
  btVector3 bodyPosition = bodyTransform.getOrigin();
  position = glm::vec3(bodyPosition.x(), bodyPosition.y(), bodyPosition.z());
}
