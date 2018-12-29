#ifndef WILT_PHYSICSENTITY_H
#define WILT_PHYSICSENTITY_H

#include <vector>

#include "Entity.h"

class PhysicsEntity;

struct PhysicsContactPoint
{
  PhysicsEntity* entity;
  btVector3 point;
};

class PhysicsEntity : public Entity
{
protected:
  btRigidBody* body;
  std::vector<PhysicsContactPoint> contactPoints;

public:
  PhysicsEntity(Model* model, const EntitySpawnInfo& info, btRigidBody* body);

public:
  btRigidBody* getBody();
  void addContactPoint(PhysicsEntity* entity, const btVector3& point);
  void resetContactPoints();

public:
  // Entity overrides
  void update(GameState& state, float time) override;

}; // class AnimatedEntity

#endif // !WILT_PHYSICSENTITY_H
