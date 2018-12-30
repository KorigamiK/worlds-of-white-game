#ifndef WILT_ANIMATEDENTITY_H
#define WILT_ANIMATEDENTITY_H

#include "Entity.h"
#include "../graphics/IAnimator.h"

class AnimatedEntity : public Entity
{
public:
  IAnimator* animator;

public:
  AnimatedEntity(Model* model, const EntitySpawnInfo& info, IAnimator* animator);

public:
  // Entity overrides
  void draw_faces(GameState& state, DepthProgram& program, float time) override;
  void draw_lines(GameState& state, LineProgram& program, float time) override;
  void draw_debug(GameState& state, Program& program, float time) override;

}; // class AnimatedEntity

#endif // !WILT_ANIMATEDENTITY_H
