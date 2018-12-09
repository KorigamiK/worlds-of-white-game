#ifndef WILT_DECORATIONINSTANCE_H
#define WILT_DECORATIONINSTANCE_H

#include "Instance.h"

class DecorationInstance : public Instance
{
public:

public:
  using Instance::Instance;

public:
  // Instance overrides
  void draw_faces(GameState& state, Program& program, float time) override;
  void draw_lines(GameState& state, Program& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;

}; // class DecorationInstance

#endif // !WILT_DECORATIONINSTANCE_H
