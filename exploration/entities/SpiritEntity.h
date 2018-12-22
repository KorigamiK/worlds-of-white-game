#ifndef WILT_SPIRITENTITY_H
#define WILT_SPIRITENTITY_H

#include "Entity.h"

class SpiritEntity : public Entity
{
public:
  glm::vec3 desiredPosition;
  glm::vec3 playerPosition;

  glm::vec3 tailPosition1;
  glm::vec3 tailPosition2;
  glm::vec3 tailPosition3;

public:
  SpiritEntity(Model* model, const EntitySpawnInfo& info);

public:
  // Entity overrides
  void update(GameState& state, float time) override;

  void draw_faces(GameState& state, Program& program, float time) override;
  void draw_lines(GameState& state, Program& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;
};

#endif // !WILT_SPIRITENTITY_H
