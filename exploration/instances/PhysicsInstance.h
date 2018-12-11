#ifndef WILT_PHYSICSINSTANCE_H
#define WILT_PHYSICSINSTANCE_H

#include "Instance.h"

class PhysicsInstance : public Instance
{
protected:
  btRigidBody* body;

public:
  PhysicsInstance(Model* model, const InstanceSpawnInfo& info, btRigidBody* body);

public:
  btRigidBody* getBody();

public:
  // Instance overrides
  void update(GameState& state, float time) override;

}; // class AnimatedInstance

#endif // !WILT_PHYSICSINSTANCE_H
