#include "SpiritEntity.h"

void SpiritEntity::update(GameState& state, float time)
{
  position += (glm::mat3)glm::rotate(glm::mat4(), rotation, { 0, 0, 1 }) * glm::vec3(0.005f, 0, 0);
  rotation += 0.005f;
  position.z = 1.0f + 0.5f * std::sin(time + rotation);
}
