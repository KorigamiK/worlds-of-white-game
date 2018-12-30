#ifndef WILT_DECORATIONENTITY_H
#define WILT_DECORATIONENTITY_H

#include "Entity.h"

class DecorationEntity : public Entity
{
public:
  enum DecorationState
  {
    DRAWN,
    HIDDEN,
    DRAWING_FAR,
    DRAWING_NEAR,
    HIDING_FAR,
    HIDING_NEAR
  };

public:
  float drawPercentage = 0.0f;
  DecorationState drawState = HIDDEN;

  glm::mat4 transform;

public:
  DecorationEntity(Model* model, const EntitySpawnInfo& info);

public:
  // Entity overrides
  void update(GameState& state, float time) override;
  void draw_faces(GameState& state, DepthProgram& program, float time) override;
  void draw_lines(GameState& state, LineProgram& program, float time) override;
  void draw_debug(GameState& state, DebugProgram& program, float time) override;

}; // class DecorationEntity

#endif // !WILT_DECORATIONENTITY_H
