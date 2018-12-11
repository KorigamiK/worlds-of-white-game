#include "AnimatedInstance.h"

AnimatedInstance::AnimatedInstance(Model* model, const InstanceSpawnInfo& info, IAnimator* animator)
  : Instance{ model, info }
  , animator{ animator }
{ }

void AnimatedInstance::draw_faces(GameState& state, Program& program, float time)
{
  animator->applyAnimation(program, time, model->joints);
  program.setFloat("draw_percentage", 1.0f);
  model->draw_faces(program, time, position, rotation, scale);
}

void AnimatedInstance::draw_lines(GameState& state, Program& program, float time)
{
  animator->applyAnimation(program, time, model->joints);
  program.setFloat("draw_percentage", 1.0f);
  model->draw_lines(program, time, position, rotation, scale);
}

void AnimatedInstance::draw_debug(GameState& state, Program& program, float time)
{

}
