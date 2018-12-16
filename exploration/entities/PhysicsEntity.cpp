#include "PhysicsEntity.h"

PhysicsEntity::PhysicsEntity(Model* model, const EntitySpawnInfo& info, btRigidBody* body)
  : Entity{ model, info }
  , body{ body }
{ }

btRigidBody* PhysicsEntity::getBody()
{
  return body;
}

void PhysicsEntity::update(GameState& state, float time)
{
  btTransform bodyTransform;
  body->getMotionState()->getWorldTransform(bodyTransform);
  btVector3 bodyPosition = bodyTransform.getOrigin();
  position = glm::vec3(bodyPosition.x(), bodyPosition.y(), bodyPosition.z());
}
