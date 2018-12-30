#include "AnimatedEntity.h"

AnimatedEntity::AnimatedEntity(Model* model, const EntitySpawnInfo& info, IAnimator* animator)
  : Entity{ model, info }
  , animator{ animator }
{ }

void AnimatedEntity::draw_faces(GameState& state, DepthProgram& program, float time)
{
  animator->applyAnimation(program, time, model->joints);
  program.setDrawPercentage(1.0f);
  model->draw_faces(program, time, model->makeEntityTransform(position, rotation, scale));
}

void AnimatedEntity::draw_lines(GameState& state, LineProgram& program, float time)
{
  animator->applyAnimation(program, time, model->joints);
  program.setDrawPercentage(1.0f);
  model->draw_lines(program, time, model->makeEntityTransform(position, rotation, scale));
}

void AnimatedEntity::draw_debug(GameState& state, Program& program, float time)
{

}
