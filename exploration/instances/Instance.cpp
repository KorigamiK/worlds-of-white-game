#include "Instance.h"

Instance::Instance(Model* model, glm::vec3 position, float rotation, IAnimator* animator, float scale)
  : model{ model }
  , position{ position }
  , rotation{ rotation }
  , animator{ animator }
  , scale{ scale }
{ }

void Instance::update(GameState& state, float time)
{

}

void Instance::draw_faces(Program& program, float time)
{
  model->draw_faces(program, time, position, rotation, scale, animator);
}

void Instance::draw_lines(Program& program, float time)
{
  model->draw_lines(program, time, position, rotation, scale, animator);
}

void Instance::draw_debug(Program& program, float time)
{

}