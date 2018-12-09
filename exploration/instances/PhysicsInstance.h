#ifndef WILT_PHYSICSINSTANCE_H
#define WILT_PHYSICSINSTANCE_H

#include "Instance.h"

class PhysicsInstance : public Instance
{
public:
  btRigidBody* body;

public:
  PhysicsInstance(Model* model, glm::vec3 position, float rotation, btRigidBody* body, float scale = 1.0f);

public:
  // Instance overrides
  void update(GameState& state, float time) override;

}; // class AnimatedInstance

#endif // !WILT_PHYSICSINSTANCE_H
