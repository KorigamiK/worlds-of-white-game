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
  
  bool dashing;
  bool dashUsed;
  std::chrono::time_point<std::chrono::high_resolution_clock> dashTime;
  glm::vec3 dashDirection;

  int spiritNext;
  std::vector<SpiritEntity*> spirits;

  // jumping logic
  std::chrono::high_resolution_clock::time_point lastTouchTime;
  bool jumpUsed;

public:
  // Entity overrides
  void update(GameState& state, float time) override;
  void draw_faces(GameState& state, DepthProgram& program, float time) override;
  void draw_lines(GameState& state, LineProgram& program, float time) override;
  void draw_debug(GameState& state, DebugProgram& program, float time) override;
};

#endif // !WILT_PLAYERENTITY_H
