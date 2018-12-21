#ifndef WILT_SPIRITENTITY_H
#define WILT_SPIRITENTITY_H

#include "Entity.h"

class SpiritEntity : public Entity
{
public:
  using Entity::Entity;

  glm::vec3 desiredPosition;
  glm::vec3 playerPosition;

public:
  // Entity overrides
  void update(GameState& state, float time) override;
};

#endif // !WILT_SPIRITENTITY_H
