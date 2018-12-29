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
public:
  enum Type
  {
    SCENERY, // terrain, solid decorations, static items
    PLAYER,
    ENEMY
  };

protected:
  btRigidBody* body;
  Type type;
  std::vector<PhysicsContactPoint> contactPoints;

public:
  PhysicsEntity(Model* model, const EntitySpawnInfo& info, btRigidBody* body, Type type);

public:
  btRigidBody* getBody();
  Type getType() { return type; }
  void addContactPoint(PhysicsEntity* entity, const btVector3& point);
  void resetContactPoints();

public:
  // Entity overrides
  void update(GameState& state, float time) override;

}; // class AnimatedEntity

#endif // !WILT_PHYSICSENTITY_H
