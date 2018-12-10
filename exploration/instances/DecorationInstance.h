#ifndef WILT_DECORATIONINSTANCE_H
#define WILT_DECORATIONINSTANCE_H

#include "Instance.h"

class DecorationInstance : public Instance
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

  // move these to a model
  float farHideDistance = 6.0f;
  float farDrawDistance = 5.0f;
  float farDrawRate = 1.0f;
  float nearHideDistance = 1.5f;
  float nearDrawDistance = 2.5f;
  float nearDrawRate = 2.0f;

public:
  using Instance::Instance;

public:
  // Instance overrides
  void update(GameState& state, float time) override;
  void draw_faces(GameState& state, Program& program, float time) override;
  void draw_lines(GameState& state, Program& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;

}; // class DecorationInstance

#endif // !WILT_DECORATIONINSTANCE_H
