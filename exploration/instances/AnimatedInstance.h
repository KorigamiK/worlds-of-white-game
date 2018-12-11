#ifndef WILT_ANIMATEDINSTANCE_H
#define WILT_ANIMATEDINSTANCE_H

#include "Instance.h"
#include "../graphics/IAnimator.h"

class AnimatedInstance : public Instance
{
public:
  IAnimator* animator;

public:
  AnimatedInstance(Model* model, const InstanceSpawnInfo& info, IAnimator* animator);

public:
  // Instance overrides
  void draw_faces(GameState& state, Program& program, float time) override;
  void draw_lines(GameState& state, Program& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;

}; // class AnimatedInstance

#endif // !WILT_ANIMATEDINSTANCE_H
