#ifndef WILT_SMASHEFFECTENTITY_T
#define WILT_SMASHEFFECTENTITY_T

#include <chrono>

#include "Entity.h"

class SmashEffectEntity : public Entity
{
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
  float growRate;

public:
  SmashEffectEntity(Model* model, const EntitySpawnInfo& info);

public:
  // Entity overrides
  virtual void update(GameState& state, float time);
  
};

#endif // !WILT_SMASHEFFECTENTITY_T
