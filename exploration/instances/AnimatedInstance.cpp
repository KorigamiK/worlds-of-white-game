#include "AnimatedInstance.h"

AnimatedInstance::AnimatedInstance(Model* model, glm::vec3 position, float rotation, IAnimator* animator, float scale)
  : Instance{ model, position, rotation, scale }
  , animator{ animator }
{ }

void AnimatedInstance::draw_faces(Program& program, float time)
{
  animator->applyAnimation(program, time, model->joints);
  model->draw_faces(program, time, position, rotation, scale);
}

void AnimatedInstance::draw_lines(Program& program, float time)
{
  animator->applyAnimation(program, time, model->joints);
  model->draw_lines(program, time, position, rotation, scale);
}

void AnimatedInstance::draw_debug(Program& program, float time)
{

}
