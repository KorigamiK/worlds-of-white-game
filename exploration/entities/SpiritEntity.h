#ifndef WILT_SPIRITENTITY_H
#define WILT_SPIRITENTITY_H

#include <chrono>

#include "Entity.h"

class SpiritEntity : public Entity
{
public:
  enum SpiritState
  {
    // The spirit is circling the player slowly
    IDLING,

    // The spirit moves swiftly towards a target until hit, or moves a limited
    // amount of time
    ATTACKING,

    // When a target is hit after attacking, the spirit moves in an arc away
    // from the hit target before making it back to the player
    RECOVERING,

    // When a target is not hit after attacking, the spirit moves swiftly and
    // directly back to the player
    RETREATING
  };

public:
  SpiritState state;

  glm::vec3 playerPosition;
  glm::vec3 tailPosition1;
  glm::vec3 tailPosition2;
  glm::vec3 tailPosition3;

  // IDLE
  float heightMax;
  float heightPeriod;
  float heightPoint;
  float anglePoint;
  float distance;

  // ATTACKING
  glm::vec3 attackTarget;

public:
  SpiritEntity(Model* model, const EntitySpawnInfo& info);

public:
  // Entity overrides
  void update(GameState& state, float time) override;

  void draw_faces(GameState& state, DepthProgram& program, float time) override;
  void draw_lines(GameState& state, LineProgram& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;

public:
  void attack(glm::vec3 direction);
};

#endif // !WILT_SPIRITENTITY_H
