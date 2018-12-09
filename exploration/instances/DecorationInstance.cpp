#include "DecorationInstance.h"

void DecorationInstance::draw_faces(GameState& state, Program& program, float time)
{
  float integ;
  float fract = std::modf(time, &integ);
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", fract);
  model->draw_faces(program, time, position, rotation, scale);
}

void DecorationInstance::draw_lines(GameState& state, Program& program, float time)
{
  float integ;
  float fract = std::modf(time, &integ);
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", fract);
  model->draw_lines(program, time, position, rotation, scale);
}

void DecorationInstance::draw_debug(GameState& state, Program& program, float time)
{

}
