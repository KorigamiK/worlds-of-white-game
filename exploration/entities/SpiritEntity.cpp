#include "SpiritEntity.h"

const auto SPIRIT_CORRECTION_RATE = 0.04f;

void SpiritEntity::update(GameState& state, float time)
{
  auto dx = desiredPosition.x - position.x;
  auto dy = desiredPosition.y - position.y;
  auto dz = desiredPosition.z - position.z;
  rotation.z = std::atan2(dy, dx);

  position = position * (1 - SPIRIT_CORRECTION_RATE) + desiredPosition * SPIRIT_CORRECTION_RATE;
}
