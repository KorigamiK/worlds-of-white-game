#include "SmashEffectEntity.h"

using namespace std::chrono_literals;

const auto SMASH_DURATION = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(0.25s);
const auto SMASH_RATE = 0.05f;

SmashEffectEntity::SmashEffectEntity(Model* model, const EntitySpawnInfo & info)
  : Entity(model, info)
  , endTime{ std::chrono::high_resolution_clock::now() + SMASH_DURATION }
  , growRate{ SMASH_RATE }
{

}

void SmashEffectEntity::update(GameState& state, float time)
{
  scale += growRate;
  
  if (endTime < std::chrono::high_resolution_clock::now())
    state.removeList.push_back(this);
}
