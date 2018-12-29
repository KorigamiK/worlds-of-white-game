#include "PhysicsEntity.h"

PhysicsEntity::PhysicsEntity(Model* model, const EntitySpawnInfo& info, btRigidBody* body, Type type)
  : Entity{ model, info }
  , body{ body }
  , type{ type }
{
  body->setUserPointer(this);
}

btRigidBody* PhysicsEntity::getBody()
{
  return body;
}

void PhysicsEntity::addContactPoint(PhysicsEntity* entity, const btVector3& point)
{
  contactPoints.push_back({ entity, point });
}

void PhysicsEntity::resetContactPoints()
{
  // TODO: try to keep capacity
  contactPoints.clear();
}

void PhysicsEntity::update(GameState& state, float time)
{
  btTransform bodyTransform;
  body->getMotionState()->getWorldTransform(bodyTransform);

  btVector3 bodyPosition = bodyTransform.getOrigin();
  position = glm::vec3(bodyPosition.x(), bodyPosition.y(), bodyPosition.z());

  float bodyRotationX;
  float bodyRotationY;
  float bodyRotationZ;
  bodyTransform.getRotation().getEulerZYX(bodyRotationZ, bodyRotationY, bodyRotationX);
  rotation = glm::vec3(bodyRotationX, bodyRotationY, bodyRotationZ);
}
