#ifndef WILT_PLAYERENTITY_H
#define WILT_PLAYERENTITY_H

#include <chrono>
#include <vector>

#include <glm/glm.hpp>

#include "PhysicsEntity.h"
#include "SpiritEntity.h"

class PlayerEntity : public PhysicsEntity
{
public:
  PlayerEntity(Model* model, const EntitySpawnInfo& info);

  glm::vec3 velocity;
  
  bool dashing;
  bool dashUsed;
  std::chrono::time_point<std::chrono::high_resolution_clock> dashTime;
  glm::vec3 dashDirection;

  std::vector<SpiritEntity*> spirits;

public:
  // Entity overrides
  void update(GameState& state, float time) override;
  void draw_faces(GameState& state, Program& program, float time) override;
  void draw_lines(GameState& state, Program& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;
};

#endif // !WILT_PLAYERENTITY_H
