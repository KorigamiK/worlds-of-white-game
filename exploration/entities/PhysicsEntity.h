#ifndef WILT_PHYSICSENTITY_H
#define WILT_PHYSICSENTITY_H

#include "Entity.h"

class PhysicsEntity : public Entity
{
protected:
  btRigidBody* body;

public:
  PhysicsEntity(Model* model, const EntitySpawnInfo& info, btRigidBody* body);

public:
  btRigidBody* getBody();

public:
  // Entity overrides
  void update(GameState& state, float time) override;

}; // class AnimatedEntity

#endif // !WILT_PHYSICSENTITY_H
