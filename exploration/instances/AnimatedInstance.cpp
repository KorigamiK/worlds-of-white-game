#include "AnimatedInstance.h"

AnimatedInstance::AnimatedInstance(Model* model, glm::vec3 position, float rotation, IAnimator* animator, float scale)
  : Instance{ model, position, rotation, scale }
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
