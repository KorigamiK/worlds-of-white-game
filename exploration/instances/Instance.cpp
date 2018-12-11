#include "Instance.h"

Instance::Instance(Model* model, const InstanceSpawnInfo& info)
  : model{ model }
  , position{ info.location}
  , rotation{ info.rotation.z } // TODO: use full rotation info
  , scale{ info.scale.x }       // TODO: use full scale info
{ }

void Instance::update(GameState& state, float time)
{

}

void Instance::draw_faces(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", 1.0f);
  model->draw_faces(program, time, position, rotation, scale);
}

void Instance::draw_lines(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", 1.0f);
  model->draw_lines(program, time, position, rotation, scale);
}

void Instance::draw_debug(GameState& state, Program& program, float time)
{

}
