#ifndef WILT_SPIRITENTITY_H
#define WILT_SPIRITENTITY_H

#include "Entity.h"

class SpiritEntity : public Entity
{
public:
  using Entity::Entity;

  float height = 1.0f;

public:
  // Entity overrides
  void update(GameState& state, float time) override;
};

#endif // !WILT_SPIRITENTITY_H
